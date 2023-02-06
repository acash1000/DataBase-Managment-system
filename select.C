#include "catalog.h"
#include "query.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;
	Status stat;
	AttrDesc attrDesc;
	AttrDesc* projNamesDesc;
	int reclen;
	Operator myOp;
	const char* filter;
	reclen = 0;

	projNamesDesc = new AttrDesc[projCnt];
	for(int i = 0 ; i < projCnt;i++){
		stat = attrCat->getInfo(projNames[i].relName,projNames[i].attrName,projNamesDesc[i]);
		reclen+= projNamesDesc[i].attrLen;
		if(stat != OK) {return stat;}
	}
	if(attr != NULL){
		stat = attrCat->getInfo(string(attr->relName),string(attr->attrName),attrDesc);
		int tmpInt;
		float tmpFloat;

		switch (attr->attrType)
		{
		case INTEGER:
			tmpInt = atoi(attrValue);
			filter = (char*)&tmpInt;
			break;
		case FLOAT:
			tmpFloat = atof(attrValue);
			filter = (char*)&tmpFloat;
		
		case STRING:
		filter = attrValue;
		break;
		}
		myOp = op;
	}else{
		strcpy(attrDesc.relName,projNames[0].relName);
		strcpy(attrDesc.attrName,projNames[0].attrName);
		attrDesc.attrOffset = 0;
		attrDesc.attrLen = 0;
		attrDesc.attrType = STRING;
		filter= NULL;
		myOp = EQ;
	}

	stat = ScanSelect(result,projCnt,projNamesDesc,&attrDesc,myOp,filter,reclen);
	if(stat != OK){return stat;}
	return OK;
}


const Status ScanSelect(const string & result, 
#include "stdio.h"
#include "stdlib.h"
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;
	Status stat;
	int resTupCount = 0;

	InsertFileScan resultRel(result,stat);
	if(stat != OK){return stat;}
	char outputData[reclen];
	Record outRec;
	outRec.data = (void *) outputData;
	outRec.length = reclen;
	HeapFileScan relScan(attrDesc->relName,stat);
	if(stat != OK){return stat;}

	stat = relScan.startScan(attrDesc->attrOffset,attrDesc->attrLen,(Datatype) attrDesc->attrType, filter, op);

	RID relRID; 
	Record relRec;
	while(relScan.scanNext(relRID) == OK){
		stat = relScan.getRecord(relRec);
		ASSERT(stat == OK);
		int outputOffset = 0;
		for(int i = 0 ; i <projCnt;i++){
			memcpy(outputData + outputOffset,(char*) relRec.data+projNames[i].attrOffset,projNames[i].attrLen);
			outputOffset += projNames[i].attrLen;
		}
		RID outRid;
		stat = resultRel.insertRecord(outRec,outRid);
		ASSERT(stat == OK);
		resTupCount ++;	
	}
	return OK;
}
