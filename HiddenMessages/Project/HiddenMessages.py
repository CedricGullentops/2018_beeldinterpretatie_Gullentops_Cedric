#########################################################
#  Author: Cedric Gullentops                            #
#                                                       #
#  Description: This is program that allows data to be  #
#  stored inside an image. Three different sorts can    #
#  be stored: a string, an other image or a bitfile.    #
#  The stored data can also be extracted. See the       #
#  examples below.                                      #
#########################################################

# examples:
# Hide a string -> string -hide "This is a String to be hidden!"
# Decode a picture -> decode -i "C:\Users\Bullseye\PycharmProjects\HiddenMessages\encoded.png"

import argparse
import cv2
import math


# ---- GENERAL ----

# Functie voor de maximale grootte van het bericht (max waarde van rgb met 2 veranderen per pixel -> 3 kleuren
# - de header size: 24 bit)
def maxmessagelenght(shape):
    return shape[0]*shape[1]*3*2-24


# Functie voor een string om te vormen naar een bit-array.
def stringtobits(string):
    result = []
    for char in string:
        strbits = bin(ord(char))[2:]  # ord: char to int, bin: int to bin, verwijder de eerste 2
        strbits = '00000000'[len(strbits):] + strbits  # splits een stuk van de 0 array en plak de bits erbij aan
        result.extend([int(bit) for bit in strbits])  # elke bit int in array
    return result


# Functie voor een bit-array om te vormen naar een string.
def bitstostring(bitarray):
    chars = []
    for b in range(len(bitarray) / 8):  # maak een array met #bits/8 getallen
        byte = bitarray[b*8:(b+1)*8]  # maak een byte van de eerste 8 bits
        chars.append(chr(int(''.join([str(bit) for bit in byte]), 2)))  # bit -> string, string in array,
        # array -> int, int -> char, char in chararray (string)
    return ''.join(chars)


# Functie om een array van bits om te zetten naar een integer
def bitstoint(bitarray):
    return int(''.join([str(bit) for bit in bitarray]), 2)


# Functie om een integer om te zetten in een array van bits.
def inttobits(number):
    arraybits = []
    convbits = bin(number)[2:]
    arraybits.extend([int(bit) for bit in convbits])
    return arraybits


# Gebruik een XOR methode om een array te encoderen of decoderen via een key.
def applykey(array):
    result = []
    key = [1, 0, 1, 1, 1, 1, 0, 1]
    if len(array) % 8 != 0:
        print "The message couldn't be encrypted because of a wrong message size!"
        exit(3)
    for b in range(len(array) / 8):
        byte = array[b * 8:(b + 1) * 8]
        for i in range(len(byte)):
            result.append(byte[i] ^ key[i])
    return result


# ---- ENCODE ----

# Maak een header van bits om de grootte van de file en mode te weten.
def createheader(mode, msglength):
    modearr = inttobits(mode)
    lengtharr = inttobits(msglength)
    while len(modearr) < 3:
        modearr = [0] + modearr
    while len(lengtharr) < 21:
        lengtharr = [0] + lengtharr
    header = modearr + lengtharr
    return header


# Verander elke rgb waarden op basis van het geencodeerde bericht.
def changevalues(bitarray):
    counter = 0
    secondrow = False
    for b in bitarray:
        if counter == img.shape[0] * img.shape[1] * 3:
            secondrow = True
            counter = 0
        value = img.item((int(math.floor(counter/3)) % img.shape[0], int(math.floor(counter/(img.shape[0]*3))),
                          counter % 3))
        colorbits = inttobits(value)
        if not secondrow:
            if colorbits[len(colorbits)-1] == b:
                counter += 1
                continue
            else:
                colorbits[len(colorbits)-1] = b
        else:
            if colorbits[len(colorbits)-2] == b:
                counter += 1
                continue
            else:
                colorbits[len(colorbits)-2] = b
        value = bitstoint(colorbits)
        img.itemset((int(math.floor(counter/3)) % img.shape[0], int(math.floor(counter/(img.shape[0]*3))),
                     counter % 3), value)
        counter += 1
    return


# Steek een bit gebaseerd bericht in een foto.
def convertimage(msglength, maxlength, mode, message):
    if msglength+24 <= maxlength and img.shape[0] > 24:
        print "The given picture is of sufficient size to store the message!"
    else:
        print "The given picture is not of sufficient size to store the message!"
        exit(2)
    header = createheader(mode, msglength)
    encoded = applykey(header+message)
    changevalues(encoded)
    cv2.imwrite("encoded.png", img) & 0xFF
    print "Message succesfully stored in encoded.png!"
    cv2.imshow('encoded', img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    exit(0)


# ---- DECODE ----

# Haal de benodigde info uit de header.
def getheaderinfo(header):
    info = []
    modebits = [header[0],  header[1],  header[2]]
    info.append(bitstoint(modebits))
    lengthbits = []
    for i in range(3, 24):
        lengthbits.append(header[i])
    info.append(bitstoint(lengthbits))
    return info


# Haal een bit-array op van de LSB van elke kleur kanaal.
def getvalues():
    header = []
    for i in range(0, 24):
        value = img.item((int(math.floor(i / 3)) % img .shape[0], int(math.floor(i / (img.shape[0] * 3))), i % 3))
        arraybits = inttobits(value)
        header.append(arraybits[len(arraybits)-1])
    header = applykey(header)
    info = getheaderinfo(header)
    data = []
    for i in range(24, 24+info[1]):
        value = img.item((int(math.floor(i / 3)) % img.shape[0], int(math.floor(i / (img.shape[0] * 3))), i % 3))
        arraybits = inttobits(value)
        data.append(arraybits[len(arraybits) - 1])
    data = applykey(data)
    return [info, data]


# Decodeer een image
def decodeimage():
    data = getvalues()
    if data[0][0] == 0:
        print "The stored data was a String:"
        print bitstostring(data[1])
    elif data[0][0] == 1:
        print "The stored data was an image"
        print "Saving image at: ..."
    elif data[0][0] == 2:
        print "The stored data was a bitfile."
        print "Saving file at: ..."
    else:
        print "Something went wrong converting the data."
    return


# Maak een commandline parser voor de gewenste mode, het item dat moet verborgen worden en een 
# image om het bericht in te steken.
parser = argparse.ArgumentParser(description="Hide a message or picture in another picture or video.")

parser.add_argument("mode", default="string", help="selects the mode to use, "
                                                   "choose either: string, picture, bitfile or decode")
parser.add_argument("-hide", "--hidden", default="Hide this string!", 
                    help="the thing to be hidden, either a path to an image/bitfile or a string")
parser.add_argument("-i", "--image", default=r"C:\Users\Bullseye\Documents\HiddenMessages\field.jpg", 
                    help="the path to a usable image or the image to decode")

args = parser.parse_args()
img = cv2.imread(args.image, 1)

cv2.imshow('image', img)
cv2.waitKey(0)

# Voer de gekozen mode uit.
if args.mode == "string":
    print "String mode was chosen."
    bits = stringtobits(args.hidden)
    length = len(bits)
    print ("The message length is: " + str(length))
    maxlen = maxmessagelenght(img.shape)
    print ("The picture can store a length of: " + str(maxlen))
    convertimage(length, maxlen, 0, bits)
elif args.mode == "picture":
    print "Picture mode was chosen."
elif args.mode == "bitfile":
    print "Bitfile mode was chosen."
elif args.mode == "decode":
    print "Decode mode was chosen."
    maxlen = maxmessagelenght(img.shape)
    print ("The picture may have stored a length of: " + str(maxlen))
    decodeimage()
else:
    print "No valid mode was given! Next time choose either string, picture, bitfile or decode!"
    exit(1)
exit(0)
