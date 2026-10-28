#!/usr/bin/env bash

set -e -o pipefail

source .github/scripts/vars.sh
source .github/scripts/fold.sh

# config
export PHP_VERSION=${PHP_VERSION:-"7.4"}
export COVERAGE=${COVERAGE:-false}
export DEBIAN_FRONTEND=noninteractive
export SUDO=sudo

function install_apt_packages() (
    ${SUDO} add-apt-repository ppa:ondrej/php
    ${SUDO} apt-get update
    ${SUDO} apt-get install -y libjson-c-dev libyaml-dev php${PHP_VERSION}-cli php${PHP_VERSION}-dev lcov
)

# the runner image ships its own php, make sure the matrix version is used
function select_php_version() (
    set -o xtrace
    ${SUDO} update-alternatives --set php /usr/bin/php${PHP_VERSION}
    ${SUDO} update-alternatives --set phpize /usr/bin/phpize${PHP_VERSION}
    ${SUDO} update-alternatives --set php-config /usr/bin/php-config${PHP_VERSION}
    php --version
)

cifold "install apt packages" install_apt_packages
cifold "select php version" select_php_version

# source and execute script used in travis
source .github/scripts/suite.sh
run_all
