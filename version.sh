#!/bin/sh

# generate version string



MAJOR=0
MINOR=0
MICRO=1
API=000000
#    |   |
#    |   +-- increment by one if interfaces were added without 
#    |       breaking the API (results in a warning)
#    +------ increment by 10000 on API changes that break older versions
#            (results in an error)


# ----------------------------------------------

# function to print helptext
function help()
{
    echo "$0 --long | --short | --major | --minor | --micro | --git | --api"
}

# output micro version
function micro()
{
    echo -n $MICRO
}

# output minor version
function minor()
{
    echo -n $MINOR
}

# output major version
function major()
{
    echo -n $MAJOR
}

# output api version
function api()
{
    echo -n $API
}

# output git version
function output_git()
{
    T=$(git describe --always --dirty)
    echo -n ${T%%"\n"*}
}

# output short version string
function output_short()
{
    echo -n $(major).$(minor).$(micro)
}

# output long version string
function output_long()
{
    echo -n $(major).$(minor).$(micro)-$(output_git)
}

# parse cmdline arguments
TEMP=`getopt -o lsMmuga --long long,short,major,minor,micro,git,api -n 'version.sh' -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true ; do
    case "$1" in
        # long
        -l|--long) output_long ; shift 1 ;;

        # short
        -s|--short) output_short ; shift 1 ;;

        # git
        -g|--git) output_git ; shift 1 ;;

        # major
        -M|--major) major ; shift 1 ;;

        # minor
        -m|--minor) minor ; shift 1 ;;

        # micro
        -u|--micro) micro ; shift 1 ;;

        # api
        -a|--api) api ; shift 1 ;;

        # ?!
        --) shift ; break ;;
        *) echo "Argument parsing error!" ; exit 1 ;;
    esac
done

