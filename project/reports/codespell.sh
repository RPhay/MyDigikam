#!/bin/bash

# SPDX-FileCopyrightText: 2013-2025 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Run CodeSpell static analyzer on whole digiKam source code.
# https://github.com/codespell-project/codespell
#
# If '--nowebupdate' is passed as argument, static analyzer results are not pushed online at
# https://files.kde.org/digikam/reports/ (default yes).
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

. ./common.sh

# Analyzer configuration.
. ../../.codespell

StartScript
checksCPUCores

if ! which codespell ; then

    echo "CodeSpell Static analyzer is not available."
    echo "Please install CodeSpell from https://github.com/codespell-project/codespell"
    echo "Aborted..."
    exit -1

else

    echo "Check CodeSpell static analyzer passed..."

fi

ORIG_WD="`pwd`"
REPORT_DIR="report.codespell"

# Get active git branches to create report description string
TITLE="digiKam-$(parseGitBranch)$(parseGitHash)"
echo "CodeSpell Static Analyzer task name: $TITLE"

rm -fr $ORIG_WD/$REPORT_DIR

# Print the skipped directories taken from the config file.

echo "CodeSpell skipped dir : $IGNORE_DIRS"
echo "CodeSpell ignore words: $CODESPELL_IGNORE_WORDS"

# Generate TXT report

codespell \
         -C 1 \
         -S "$IGNORE_DIRS" \
         -L $CODESPELL_IGNORE_WORDS \
         ../../ \
         1> ./codespell-trace.txt

# NOTE: return value is not documented and not suitable.

if [[ -f ./codespell-trace.txt ]] ; then

    ISSUES=$(grep -c '^> ' $1/codespell-trace.txt)
    mv ./codespell-trace.txt $REPORT_DIR

    # Create an HTML report

    TRACE_FILE="$REPORT_DIR/codespell-trace.txt"
    HTML_FILE="index.html"

    # HTML header

    cat > "$HTML_FILE" << 'EOF'
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>CodeSpell Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                h1 { color: #333; }
                table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #f2f2f2; }
                tr:nth-child(even) { background-color: #f9f9f9; }
                .error { color: red; font-weight: bold; }
                .file { color: blue; }
                .line { color: green; }
            </style>
        </head>
        <body>
            <h1>CodeSpell Report for __TITLE__</h1>
            <table>
                <thead>
                    <tr>
                        <th>File</th>
                        <th>Line</th>
                        <th>Error</th>
                        <th>Suggestion</th>
                    </tr>
                </thead>
                <tbody>
EOF

    # Read trace file and generate the spreadsheet HTML

    while IFS= read -r line ; do

        # Ignore lines without "==>"

        if [[ "$line" == *"==>"* ]] ; then

            # Uses awk to extract all info

            file=$(echo "$line" | awk -F':' '{print $1}')
            line_num=$(echo "$line" | awk -F':' '{print $2}' | awk '{print $1}')
            error=$(echo "$line" | awk -F'==' '{print $1}' | awk '{print $NF}')
            suggestion=$(echo "$line" | awk -F'==' '{print $2}' | awk '{print $2}')

            # ESC special char for the HTML code

            file_escaped=$(echo "$file" | sed 's/&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g; s/"/\&quot;/g; s/'\''/\&#39;/g;')
            error_escaped=$(echo "$error" | sed 's/&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g; s/"/\&quot;/g; s/'\''/\&#39;/g;')
            suggestion_escaped=$(echo "$suggestion" | sed 's/&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g; s/"/\&quot;/g; s/'\''/\&#39;/g;')

            # Append a line to the spreadsheet

            cat >> "$HTML_FILE" << EOF
                <tr>
                    <td class="file">$file_escaped</td>
                    <td class="line">$line_num</td>
                    <td class="error">$error_escaped</td>
                    <td>$suggestion_escaped</td>
                </tr>
EOF
        fi

    done < "$TRACE_FILE"

    # HTML footer

    cat >> "$HTML_FILE" << 'EOF'
        </tbody>
            </table>
            <p>Total errors: <span id="total-errors"></span></p>
            <script>
                // Counts total of errors
                const rows = document.querySelectorAll('tbody tr');
                document.getElementById('total-errors').textContent = rows.length;
            </script>
        </body>
    </html>
EOF

    sed -i "s|__TITLE__|$TITLE|g" $HTML_FILE
    mv $1/$HTML_FILE $REPORT_DIR

    if [[ $1 != "--nowebupdate" ]] ; then

        cd $ORIG_WD
        updateOnlineReport "codespell" $REPORT_DIR $TITLE $(parseGitBranch)

    fi

fi

cd $ORIG_WD

TerminateScript
