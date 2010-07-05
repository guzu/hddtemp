#!/bin/sh

interval=300
tmpdir="/tmp/results"
drive="/dev/hda"

if [ ! -d "$tmpdir" ] ; then
  echo "no such dir: $tmpdir"
  exit 1
fi

logger -s -t hddtemp "starting hddtemp monitor: interval=$interval, tmpdir=$tmpdir, drive=$drive"
stamp=`date +%s`
tmpfile_old="$tmpdir/hddtemp-$stamp"
hddtemp $drive --debug > "$tmpfile_old"

while [ 1 ] ; do
  sleep $interval
  stamp=`date +%s`
  tmpfile_new="$tmpdir/hddtemp-$stamp"
  hddtemp $drive --debug > "$tmpfile_new"
  RETURNED=`diff "$tmpfile_old" "$tmpfile_new"`
  if [ -n "$RETURNED" ] ; then
    logger -s -t hddtemp "change $tmpfile_new !!!"
    tmpfile_old="$tmpfile_new"
  else
    logger -s -t hddtemp "no change"
    rm "$tmpfile_new"
  fi
done
