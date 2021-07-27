/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/

#include <stdio.h>
#include "../regler/vectorregler.h"
#include "functionsvectorWXYZ.h"
#include <spu_mfcio.h>
#include "simpleDMA.h"
#include "regler.h"

control_block cb __attribute__ ((aligned (128)));
float data[DATA_BUFFER_ENTRIES/2] __attribute__ ((aligned (128)));
NewVectorVar VectorControllerVar;

int main(unsigned long long speid, addr64 argp, addr64 envp) {
	int i;
	unsigned int msg=0;
	UNUSED(envp);
	UNUSED(speid);

	VecVarInit(&VectorControllerVar);

#ifdef DEBUG 
	printf("Hi, this is SPU #:0x%llx with program spureglerWXYZ\n", speid);
#endif

	while (msg==0) {
		msg=spu_read_in_mbox();
		if (msg==3)
			exit(0);

		mfc_get(&cb, argp.ull, sizeof(cb), 31, 0, 0);
		mfc_write_tag_mask(1<<31);
		mfc_read_tag_status_all();
		mfc_get(data, cb.addr, DATA_BUFFER_SIZE, 31, 0, 0);
		mfc_read_tag_status_all();

		for (i=0; i<4; i++) {
			//VectorControllerVar.arraycontroller.root_eLXYZ[i]=data[i];//not used
			VectorControllerVar.arraycontroller.root_eWXYZ[i]=data[i];
			//VectorControllerVar.arraycontroller.root_vWXYZ[i]=data[i+8];//not used
		}

		MdlOutputsVectorWXYZ(&VectorControllerVar);

		for (i=0; i<4; i++) {
			//data[i]=VectorControllerVar.arraycontroller.rtY_root_yLXYZ[i];//not used
			data[i]=VectorControllerVar.arraycontroller.rtY_root_yWXYZ[i];
			//data[i+8]=VectorControllerVar.arraycontroller.root_vWXYZ[i];//not used
		}

		mfc_put(data, cb.addr, DATA_BUFFER_SIZE, 31, 0, 0);
		mfc_read_tag_status_all();

		msg=0;
		spu_write_out_mbox(msg);
	}

	exit(0);
}
