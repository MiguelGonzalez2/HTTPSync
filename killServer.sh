##WARNING. THIS IS A UTILITY SCRIPT USED FOR DEBUGGING
##It should never be used to turn off the server
#!/bin/bash
pid=$(ps aux | grep ./server | grep ? | awk '{print $2}')
kill -INT $pid
