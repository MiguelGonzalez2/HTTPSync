args = input()
if(args.find('&') != -1):
    print("Introduzca exclusivamente el parametro user")
else:
    args = args.split('=')
    if(args[0] != "user"):
        print("Introduzca exclusivamente el parametro user")
    else:    
        print("Hola "+ args[1] + "!")
