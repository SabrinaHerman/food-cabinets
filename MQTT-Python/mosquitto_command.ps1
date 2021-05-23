$myCommand = "`"C:\Program Files\mosquitto\mosquitto_sub.exe`" " + `
"-h us-west.thethings.network -t +/devices/+/up" + `
"-u yourAppHere" + `
"-P yourAccessKeyHere -v"