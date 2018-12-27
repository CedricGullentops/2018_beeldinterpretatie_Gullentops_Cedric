import argparse
import cv2
import numpy as np
import math


# examples:
# Hide a string -> string -hide "This is a String to be hidden!"
# Decode a picture -> decode -i "C:\Users\Bullseye\PycharmProjects\HiddenMessages\encoded.jpg"


#---- GENERAL ----

#Functie voor de maximale grootte van het bericht (max waarde van rgb met 2 veranderen per pixel -> 3 kleuren - de header size: 24 bit)
def maxMessageLenght(shape):
    return shape[0]*shape[1]*3*2-24


#Functie voor een string om te vormen naar een bit-array.
def stringToBits(string):
    result = []
    for char in string:
        bits = bin(ord(char))[2:]  #ord: char to int, bin: int to bin, verwijder de eerste 2
        bits = '00000000'[len(bits):] + bits #splits een stuk van de 0 array en plak de bits erbij aan
        result.extend([int(bit) for bit in bits]) #elke bit int in array
    return result


#Functie voor een bit-array om te vormen naar een string.
def bitsToString(bits):
    chars = []
    for b in range(len(bits) / 8): #maak een array met #bits/8 getallen
        byte = bits[b*8:(b+1)*8]  #maak een byte van de eerste 8 bits
        chars.append(chr(int(''.join([str(bit) for bit in byte]), 2))) #bit -> string, string in array, array -> int, int -> char, char in chararray (string)
    return ''.join(chars)


#Functie om een array van bits om te zetten naar een integer
def bitsToInt(bits):
    return int(''.join([str(bit) for bit in bits]), 2)


#Functie om een integer om te zetten in een array van bits.
def intToBits(number):
    bits = []
    convBits = bin(number)[2:]
    bits.extend([int(bit) for bit in convBits])
    return bits


#Gebruik een XOR methode om een array te encoderen of decoderen via een key.
def applyKey(array):
    result = []
    key = [1, 0, 1, 1, 1, 1, 0, 1]
    if len(array)%8 != 0:
        print "The message couldn't be encrypted because of a wrong message size!"
        exit(3)
    for b in range(len(array) / 8):
        byte = array[b * 8:(b + 1) * 8]
        for i in range(len(byte)):
            result.append(byte[i] ^ key[i])
    return result


#---- ENCODE ----

#Maak een header van bits om de grootte van de file en mode te weten.
def createHeader(mode, msgLength):
    modeArr = intToBits(mode)
    lengthArr = intToBits(msgLength)
    while len(modeArr) < 3:
        modeArr = [0] + modeArr
    while len(lengthArr) < 21:
        lengthArr = [0] + lengthArr
    header = modeArr + lengthArr
    return header


#Verander elke rgb waarden op basis van het geencodeerde bericht.
def changeValues(bits, img, msgLength):
    counter = 0
    secondRow = False
    for b in bits:
        if counter == img.shape[0] * img.shape[1] * 3:
            secondRow = True
            counter = 0
        value = img.item((int(math.floor(counter/3)) % img.shape[0], int(math.floor(counter/(img.shape[0]*3))), counter % 3))
        bits = intToBits(value)
        if not secondRow:
            if bits[len(bits)-1] == b:
                counter += 1
                continue
            else:
                bits[len(bits)-1] = b
        else:
            if bits[len(bits)-2] == b:
                counter += 1
                continue
            else:
                bits[len(bits)-2] = b
        value = bitsToInt(bits)
        img.itemset((int(math.floor(counter/3)) % img.shape[0], int(math.floor(counter/(img.shape[0]*3))), counter % 3), value)
        counter += 1
    return


#Steek een bit gebaseerd bericht in een foto.
def convertImage(msgLength, maxLength, mode, img, message):
    if msgLength+24 <= maxLength and img.shape[0]>24:
        print "The given picture is of sufficient size to store the message!"
    else:
        print "The given picture is not of sufficient size to store the message!"
        exit(2)
    header = createHeader(mode, msgLength)
    encoded = applyKey(header+message)
    changeValues(encoded, img, msgLength)
    cv2.imwrite('encoded.jpg', img)
    print "Message succesfully stored in encoded.jpg!"
    return


#---- DECODE ----

#Haal de benodigde info uit de header.
def getHeaderInfo(header):
    info = []
    modeBits = [header[0],  header[1],  header[2]]
    info.append(bitsToInt(modeBits))
    print info[0]
    lengthBits = []
    for i in range(3,24):
        lengthBits.append(header[i])
    info.append(bitsToInt(lengthBits))
    print info[1]
    return info


#Haal een bit-array op van de LSB van elke kleur kanaal.
def getValues(img):
    header = []
    for i in range(0,24):
        value = img.item((int(math.floor(i / 3)) % img.shape[0], int(math.floor(i / (img.shape[0] * 3))), i % 3))
        bits = intToBits(value)
        header.append(bits[len(bits)-1])
    print header
    header = applyKey(header)
    print header
    info = getHeaderInfo(header)


#Decodeer een image
def decodeImage(img, max):
    getValues(img)
    return


#Maak een commandline parser voor de gewenste mode, het item dat moet verborgen worden en een image om het bericht in te steken.
parser = argparse.ArgumentParser(description="Hide a message or picture in another picture or video.")

parser.add_argument("mode", default="string", help="selects the mode to use, choose either: string, picture, bitfile or decode")
parser.add_argument("-hide", "--hidden", default="Hide this string!", help="the thing to be hidden, either a path to an image/bitfile or a string")
parser.add_argument("-i", "--image", default=r"C:\Users\Bullseye\Documents\HiddenMessages\field.jpg", help="the path to a usable image or the image to decode")

args = parser.parse_args()
img = cv2.imread(args.image,-1)

cv2.imshow('image',img)
cv2.waitKey(0)

#Voer de gekozen mode uit.
if args.mode == "string":
    print "String mode was chosen."
    bits = stringToBits(args.hidden)
    length = len(bits)
    print ("The message length is: " + str(length))
    max = maxMessageLenght(img.shape)
    print ("The picture can store a length of: " + str(max))
    convertImage(length, max, 0, img, bits)
    cv2.imshow('encoded', img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
elif args.mode == "picture":
    print "Picture mode was chosen."
elif args.mode == "bitfile":
    print "Bitfile mode was chosen."
elif args.mode == "decode":
    print "Decode mode was chosen."
    max = maxMessageLenght(img.shape)
    print ("The picture may have stored a length of: " + str(max))
    decodeImage(img, max)
else:
    print "No valid mode was given! Next time choose either string, picture, bitfile or decode!"
    exit (1)

exit (0)