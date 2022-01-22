#
# Regular cron jobs for the dream package
#
0 4	* * *	root	[ -x /usr/bin/dream_maintenance ] && /usr/bin/dream_maintenance
