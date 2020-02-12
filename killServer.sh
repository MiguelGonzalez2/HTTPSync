#!/bin/bash
pid=$(ps aux | grep ./server | grep ? | awk '{print $2}')
kill -INT $pid
