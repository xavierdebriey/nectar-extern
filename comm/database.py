import time
import datetime
import struct

from pymongo import MongoClient

import definitions as d


def getTypeOfPackage(package):
    if package[:3].decode("ascii") == 'dat':
        return 'data'
    elif package[:3].decode("ascii") == 'rep':
        return 'res'
    elif package[:3].decode("ascii") == 'log':
        return 'log'
    elif package[:3].decode("ascii") == 'ack':
        return 'ack'
    else:
        return None

def bytes2measures(bytesPackage):
    bytesPackageCopy = bytesPackage
    nbOfMeasures = bytesPackageCopy[1]

    measures = []

    for i in range(nbOfMeasures):
        actualMeasure = bytesPackageCopy[i*(d.SIZEOFDATA):(i+1)*(d.SIZEOFDATA)]
        unpackedMeasure = struct.unpack('ffffffff', actualMeasure)
        newMeasure = {}
        newMeasure['timestamp'] = unpackedMeasure[0]
        newMeasure['temperature'] = unpackedMeasure[1]
        newMeasure['humidity'] = unpackedMeasure[2]
        newMeasure['weigth'] = unpackedMeasure[3]
        newMeasure['volume'] = unpackedMeasure[4]
        newMeasure['freqs'] = unpackedMeasure[5:]
        measures.append(newMeasure)

    return measures

def bytes2config(bytesPackage):
    bytesPackageCopy = bytesPackage

    config = {}

    configList = struct.unpack('IIIII', bytesPackageCopy) 

    config['measure'] = configList[0]
    config['sending'] = configList[1]
    config['freqs'] = configList[2:]

    return config

def bytes2log(bytesPackage):
    bytesPackageCopy = bytesPackage
    return bytesPackageCopy.decode("ascii")

def blen(bytesPackage):
    for i in range(len(bytesPackage)-1):
        if chr(bytesPackage[i]) == '\n' and chr(bytesPackage[i+1]) == '\n':
            return i
    return 0

def getPackagesFromBytes(bytesPackage):
    bytesPackageCopy = bytesPackage
    packages = []

    while len(bytesPackageCopy) > 0:
        typeOfPackage = getTypeOfPackage(bytesPackageCopy)
        lenOfPackage = blen(bytesPackageCopy)

        if lenOfPackage == 0:
            break

        bytes2translate = bytesPackageCopy[3:lenOfPackage]
        bytesPackageCopy = bytesPackageCopy[lenOfPackage+2:]

        if typeOfPackage == 'data':
            contentOfPackage = bytes2measures(bytes2translate)

        elif typeOfPackage == 'res':
            contentOfPackage = bytes2config(bytes2translate)

        elif typeOfPackage == 'log':
            contentOfPackage = bytes2log(bytes2translate)

        elif typeOfPackage == 'ack':
            contentOfPackage = 'ok'

        packages.append({'type_': typeOfPackage, 
                         'content': contentOfPackage})

    return packages, bytesPackageCopy

def openMongoDB(nameOfDatabase):
    mongoClient = MongoClient()
    return mongoClient[nameOfDatabase]

def storeMeasuresInDatabase(db, data, timestamp):
    ndata = data.copy()
    ntimestamp = timestamp + ndata['timestamp']
    del ndata['timestamp']
    to_db = {'time': datetime.datetime.fromtimestamp(ntimestamp),  
             'data': ndata}
    measures = db.measures
    return measures.insert_one(to_db)
