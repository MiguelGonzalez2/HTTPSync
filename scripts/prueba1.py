args = input()
if(args.find('&') != -1):
    print("Introduzca exclusivamente el parametro user\r")
else:
    args = args.split('=')
    if(args[0] != "user"):
        print("Introduzca exclusivamente el parametro user\r")
    else:    
        print("Hola "+ args[1] + "!\r")
