#!/usr/bin/env python
from getpass import getpass
from os.path import getsize, isfile, join, dirname
from os import listdir
from sys import argv

# requires pygithub3:
# pip install pygithub3
from pygithub3 import Github

USER = 'mani-monaj'
REPO = 'grSim'
GH = None


def login():
    """Authenticates."""
    login = raw_input('Login: ')
    passw = getpass()
    gh = Github(login=login, password=passw, user=USER, repo=REPO)
    #TODO: check login
    return gh


def deploy(filepath):
    """Deploys file under filepath."""
    filename = filepath.split('\\')[-1].split('/')[-1]
    filesize = getsize(filepath)
    desc = raw_input('Description: ')
    print 'Creating resource...'
    download = GH.repos.downloads.create(dict(name=filename, size=filesize, description=desc))
    print 'Uploading...'
    download.upload(filepath)


def deploy_all(dirpath):
    """Deploys all files inside dirpath that are not already deployed."""
    # List all files that are not hidden
    #TODO: think about how's that gonna work on windows
    files = [f for f in listdir(dirpath) if f[0] != '.']
    # Get all downloads
    downloads = [d.name for d in GH.repos.downloads.list().all()]
    # Deploy all files that are not in downloads
    for f in files:
        if f not in downloads:
            print 'Deploying {0}...'.format(f)
            deploy(join(dirpath, f))
        else:
            print '{0} is already deployed.'.format(f)


def print_usage():
    print """Usage:
./upload.py /path/to/file (will deploy a single file)
./upload.py --all (will deploy everything under ./dist)
"""


if __name__ == '__main__':
    if len(argv) != 2:
        print_usage()
    elif argv[1] == '--all':
        try:
            GH = login()
            dirpath = join(dirname(__file__), 'dist')
            deploy_all(dirpath)
            print 'Done.'
        except KeyboardInterrupt:
            print '\nExited.'
    elif isfile(argv[1]):
        try:
            GH = login()
            deploy(argv[1])
            print 'Done.'
        except KeyboardInterrupt:
            print '\nExited.'
    else:
        print 'File not found.'
        print_usage()

