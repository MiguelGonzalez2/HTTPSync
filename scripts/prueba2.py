def celsiusToFahrenheit(c):
    return(float(c) *1.8 +32)
args = input()
if(args.find('&') != -1):
    print("Introduzca exclusivamente el parametro celsius\r")
else:
    args = args.split("=")
    if(args[0] != "celsius"):
        print("Introduzca exclusivamente el parametro celsius\r")
    else:    
        print("Temperatura correspondiente a " + args[1] + "C en Farenheit: " + str(celsiusToFahrenheit(args[1])) + "\r")
