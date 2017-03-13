import time
import datetime
import struct

from pymongo import MongoClient

from definitions import SIZEOFDATA, SIZEOFRESPONSE, SIZEOFACK


def getTypeOfPackage(package):
    if chr(package[0]) == 'd':
        return 'data'
    elif chr(package[0]) == 'r':
        return 'res'
    elif chr(package[0]) == 'a':
        return 'ack'

def bytes2measures(bytesPackage):
    bytesPackageCopy = bytesPackage
    nbOfMeasures = bytesPackageCopy[1]
    bytesPackageCopy = bytesPackageCopy[3:-1]

    measures = []

    for i in range(nbOfMeasures):
        actualMeasure = bytesPackageCopy[i*(SIZEOFDATA-4):(i+1)*(SIZEOFDATA-4)]
        unpackedMeasure = struct.unpack('ffffffff', actualMeasure)
        newMeasure = {}
        newMeasure['timestamp'] = unpackedMeasure[0]
        newMeasure['temperature'] = unpackedMeasure[1]
        newMeasure['humidity'] = unpackedMeasure[2]
        newMeasure['weigth'] = unpackedMeasure[3]
        newMeasure['volume'] = unpackedMeasure[4]
        newMeasure['freqs'] = unpackedMeasure[5:]
        measures.append(newMeasure)

    return datas

def bytes2config(bytesPackage):
    bytesPackageCopy = bytesPackage

    config = {}

    configList = struct.unpack('IIIII', bytesPackageCopy[1:-1]) 

    config['measure'] = configList[0]
    config['sending'] = configList[1]
    config['freqs'] = configList[2:]

    return config

def getPackagesFromBytes(bytesPackage):
    bytesPackageCopy = bytesPackage
    packages = []

    while len(bytesPackageCopy) > 0:
        typeOfPackage = getTypeOfPackage(bytesPackageCopy)

        if typeOfPackage == 'data':
            numberOfMeasures = bytesPackageCopy[1]
            contentOfPackage = bytes2measures(bytesPackageCopy[:4+(SIZEOFDATA-4)*numberOfMeasures])
            bytesPackageCopy = bytesPackageCopy[4+(SIZEOFDATA-4)*numberOfMeasures:]

        elif typeOfPackage == 'res':
            contentOfPackage = bytes2config(bytesPackageCopy[:SIZEOFRESPONSE])
            bytesPackageCopy = bytesPackageCopy[SIZEOFRESPONSE:]

        elif typeOfPackage == 'ack':
            contentOfPackage = 'ok'
            bytesPackageCopy = bytesPackageCopy[SIZEOFACK:]

        packages.append({'type_': type_, 
                         'content': content})

    return packages

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
