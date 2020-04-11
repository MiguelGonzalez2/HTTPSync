def celsiusToFahrenheit(c):
    return(int(c) *1.8 +32)
args = input()
if(args.find('&') != -1):
    print("Introduzca exclusivamente el parametro celsius")
else:
    args = args.split("=")
    if(args[0] != "celsius"):
        print("Introduzca exclusivamente el parametro celsius")
    else:    
        print("Temperatura correspondiente a " + args[1] + "C en Farenheit: " + celsiusToFahrenheit(args[1]))
