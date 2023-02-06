#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
Status stat;
int relAttrCnt;
AttrDesc* relAttributes; 
InsertFileScan resultRel(relation,stat);
if(stat !=OK){return stat;}
stat = attrCat->getRelInfo(relation,relAttrCnt,relAttributes);
if(stat != OK){return stat;}
int reclen = 0; 
for(int i = 0; i< relAttrCnt;i++){
	reclen += relAttributes[i].attrLen;
}

char outputData[reclen];
Record outputRec; 
outputRec.data = (void*) outputData;
outputRec.length = reclen;

for(int i = 0 ; i<attrCnt;i++){
	for(int j = 0 ; j <relAttrCnt;j++){
		if(strcmp(relAttributes[j].attrName,attrList[i].attrName)==0){
			if(attrList[i].attrValue == NULL){
				return ATTRTYPEMISMATCH;
			}
			char* actualAttrVal;
			int tmpint;
			float tmpFloat;
			switch(attrList[i].attrType){
				case INTEGER:
				tmpint = atoi((char*)attrList[i].attrValue);
				actualAttrVal = (char*)&tmpint;
				break;
				case FLOAT:
				tmpFloat= atof((char*)attrList[i].attrValue);
				actualAttrVal = (char*)&tmpFloat;
				break;
				case STRING:
				actualAttrVal = (char*)attrList[i].attrValue;
				break;
			}
			memcpy(outputData+relAttributes[j].attrOffset,actualAttrVal,relAttributes[j].attrLen);
		}
	}
}
RID outRid;
stat = resultRel.insertRecord(outputRec,outRid);
if(stat != OK){return stat;}
// part 6
return OK;

}

