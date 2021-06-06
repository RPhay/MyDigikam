COMMIT POLICY

Everybody is welcome to committing small fixes and one-liners
without prior notification to the maintainer, provided that the
following rules are followed:

    1. Please keep your commits as small and as atomic as possible.
    2. Do not push both formatting and code changes in the same commit.
    3. Do not fix coding style and code issues in the same commit.

For larger commits, please use Gitlab functionalities or send a patch to
[bugzilla](https://bugs.kde.org/describecomponents.cgi?product=digikam).
A rule of thumb to check whether your commit is a major commit is
if it affects more than 5 lines of code.

Break down larger fixes into smaller commits. Even if you push the
commits with one "git push", git preserves your commit info.

For the todo list, see bugzilla for details which is the complete story of the project.

CODING STYLE

See the coding style detailled at this [Doxygen page](https://www.digikam.org/api/index.html#codecontrib).

DOCUMENTATION

See The API documentation to generate with Doxygen. You will find all information about the dependencies,
the configuration options, the rules to compile and install, and all pointers to contribute to the project as a developer.

Install Doxygen and Graphviz, and then run make doc from build directory. You can also run doxygen command line tool
directly from this folder or consult the Mainpage.dox file.

[Online version](https://www.digikam.org/api/index.html)

