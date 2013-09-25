#!/usr/bin/env python

# curl -i "https://api.github.com/repos/cooperhewitt/Planetary/issues"

import os
import os.path
import sys 
import urllib2
import json
import time

if __name__ == '__main__':

    whoami = os.path.realpath(sys.argv[0])
    bin = os.path.dirname(whoami)
    root = os.path.dirname(bin)

    issues = os.path.join(root, 'issues')

    for state in ('open', 'closed'):

        try:
            req = 'https://api.github.com/repos/cooperhewitt/Planetary/issues?state=%s' % state
            rsp = urllib2.urlopen(req)
        except Exception, e:
            print "failed to retrieve %s because %s" % (req, e)
            continue

        data = json.load(rsp)
        
        for i in data:
    
            for extra in ('events', 'comments'):

                key = '%s_url' % extra
                url = i.get(key, None)

                if url:

                    try:
                        extra_rsp = urllib2.urlopen(url)
                        extra_data = json.load(extra_rsp)                        

                        i[extra] = extra_data

                    except Exception, e:
                        print "failed to include %s because %s" % (url, e)
                        
            fname = "%s.json" % i['id']
            path = os.path.join(issues, fname)

            print path

            fh = open(path, 'w')
            json.dump(i, fh, indent=2)
            fh.close()

            time.sleep(2)
