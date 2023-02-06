#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
	Status stat;
	AttrDesc attrDesc;
	const char* filter;
	int resultTupCnt = 0;
	RID relRID;
	HeapFileScan relScan(relation,stat);
	if(stat !=OK){
		return stat;
	}
	if(attrName.length() == 0){
		stat = relScan.startScan(0,0,STRING,NULL,EQ);
		if(stat != OK){
			return stat;
		}
		while(relScan.scanNext(relRID) == OK){
			stat = relScan.deleteRecord();
			if(stat!= OK){
				return stat;
			}
			resultTupCnt++;
		}
		return OK;
	}
	stat = attrCat->getInfo(relation,attrName,attrDesc);
	if(stat != OK){
		return stat;
	}
	int tmpint;
	float tmpFloat;
	switch(type){
		case INTEGER:
		tmpint = atoi((char*)attrValue);
		filter = (char*)&tmpint;
		break;
		case FLOAT:
		tmpFloat= atof((char*)attrValue);
		filter = (char*)&tmpFloat;
		break;
		case STRING:
		filter = (char*)attrValue;
		break;
	}

	stat = relScan.startScan(attrDesc.attrOffset,attrDesc.attrLen,type,filter,op);
	if(stat != OK){
		return stat;
	}

	while (relScan.scanNext(relRID) == OK)
	{
		stat = relScan.deleteRecord();
		if(stat != OK){return stat;}
		resultTupCnt ++;
	}
	

// part 6
return OK;



}


