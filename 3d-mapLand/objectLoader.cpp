#include <iostream>
#include "objectLoader.h"

using namespace std;



component *v,*vn,*vt;
M3DVector3f *Ver;
M3DVector3f *Normals;
M3DVector2f *vTexCoords; 

void find_word(char compare_with[10],FILE *fp)
{
	char word[50];
	while(1)
	{
		fscanf(fp,"%s",word);
		if(!(strcmp(word,compare_with)))
		{
			break;
		}	
	}
}

void store_in_array(char compare_with[10],component *arr,FILE *fp)
{
	char word[50];
	float number;
	find_word(compare_with,fp);
	int i=0;

	// to store the vertices in v[1000]
	while(1)															
	{
		fscanf(fp,"%f",&number);
		arr[i].x=number;	
		
		fscanf(fp,"%f",&number);
		arr[i].y=number;	
		

		fscanf(fp,"%f",&number);
		arr[i].z=number;

	
		fscanf(fp,"%s",&word);
		if(!(strcmp(word,"#")))
		{
			break;
		}
		i=i+1;
	}
}
void readFile(FILE *fp)
{	
	 store_in_array("v",v,fp);
	 store_in_array("vn",vn,fp);
	 store_in_array("vt",vt,fp);
}
int insertFinal(FILE *fp,M3DVector3f *Ver,M3DVector3f *Normals,M3DVector2f *vTexCoords)
{
	int index_v,index_vn,index_vt,i=0,newline=0,total_ver=0;
	char slash,f_or_new,test[20];
	
	
	find_word("f",fp);
	
	while(1)
	{
		total_ver=total_ver+1;

		fscanf(fp,"%d",&index_v);
		fscanf(fp,"%c",&slash);	
		fscanf(fp,"%d",&index_vt);
		fscanf(fp,"%c",&slash);	
		fscanf(fp,"%d",&index_vn);
		fscanf(fp,"%c",&slash);		
	

		index_v=index_v-1;
		index_vn=index_vn-1;
		index_vt=index_vt-1;

		Ver[i][0]=v[index_v].x;		
		Ver[i][1]=v[index_v].y;
		Ver[i][2]=v[index_v].z;
		
		Normals[i][0]=vn[index_vn].x;
		Normals[i][1]=vn[index_vn].y;
		Normals[i][2]=vn[index_vn].z;

		vTexCoords[i][0]  = vt[index_vt].x;
		// mirror image occurs due to some difference b/w opengl and 3ds tex coord coordinates
		vTexCoords[i][1]= 1.0f - vt[index_vt].y;				

		i=i+1;
		
		newline=newline+1;

		if(newline==3)
		{
			newline=0;
			fscanf(fp,"%c",&slash);	
			fscanf(fp,"%c",&slash);	
			if(slash=='#')
			{
				break;
			}
		}
	}
	
	return total_ver;
}
int findSizes(FILE *fp,char to_find[10])
{
	int n;
	find_word(to_find,fp);
	find_word("#",fp);
	fscanf(fp,"%d",&n);
	return n;
}

int loadMesh(FILE *fp)
{
	
	int noVerts=findSizes(fp,"v");
	int noNormals=findSizes(fp,"vn");
	int noTextures=findSizes(fp,"vt");

	v=new component[noVerts];
	vn=new component[noNormals];
	vt=new component[noTextures];

	fseek(fp, 0, SEEK_SET);

	readFile(fp);
	int faces=findSizes(fp,"f");
	int verticesInModel=faces*3;

	Ver=new M3DVector3f[verticesInModel];
	Normals=new M3DVector3f[verticesInModel];
	vTexCoords=new M3DVector2f[verticesInModel];
	
	fseek(fp, 0, SEEK_SET);
	int total_ver=insertFinal(fp,Ver,Normals,vTexCoords);
	
	return total_ver;
}