# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi


eval `SSH_ASKPASS=/usr/bin/ksshaskpass /usr/bin/keychain -q --eval --noask --agents ssh ~/.ssh/id_rsa`

export GITSLAVE=.gitslave

alias m='make -j4'
alias v='valgrind --tool=memcheck --leak-check=full --error-limit=no '
