#!/bin/sh
echo starting
while true; do
  echo rsyncing
  rsync -avzP --files-from=7push.lst . pi@192.168.1.20:~/SpaceCorn/
  inotifywait -r -e modify --fromfile 7push.lst
done
