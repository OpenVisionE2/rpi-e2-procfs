/*
 * e2_procfs_bus.c
 *
 * (c) 2015 SIP-Online
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "e2_procfs.h"

#include <linux/dvb/frontend.h>
#include <media/dvbdev.h>

#define DVB_MAX_FRONTEND 8


int e2procfs_nim_sockets_write(struct ProcWriteInfo *proc_info, char *kbuf)
{
	proc_info->bpage = kbuf;

	return 0;
}
int e2procfs_nim_sockets_show(struct seq_file *m, void* data)
{
	int userspace = 0;
	int TunerType_S = 0;
	int TunerType_C = 1;
	int TunerType_T = 2;
	struct ProcWriteInfo *proc_info = m->private;
	char *ArrayDVBTunerType[] =  { "UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN","UNKNOWN"};
	char **DVBTunerType;
	DVBTunerType = ArrayDVBTunerType;
		DVBTunerType[TunerType_S]="DVB-S";
		DVBTunerType[TunerType_C]="DVB-C";
		DVBTunerType[TunerType_T]="DVB-T";
/*	DVB-S2 is a special case of DVB-S and is tested from frontend capabilities	*/

	if (proc_info->count > 0)
	{
		seq_printf(m, "%s\n", proc_info->bpage);
	}
	else
	{
 		struct file* fe_fd = NULL;
 		int adapter_num = 0, nsocket_index = 0;
 		char devstr[MAX_CHAR_LENGTH];

/*	User Space nim_socket  (not enabled by default) */
		if ( 	userspace == 1)	{
			seq_printf(m,
			"NIM Socket 0:\n"
			"\tType: DVB-T2\n"
			"\tName: Availink avl6862\n"
			"\tHas_Outputs: no\n"
			"\tFrontend_Device: 0\n"
			);
		} 
		else	{
 			while (adapter_num < DVB_MAX_ADAPTERS)
 			{
 				int frontend_num = 0;

	 			while (frontend_num < DVB_MAX_FRONTEND)
 				{
 					int bytes = 0;

 					bytes = sprintf(devstr, "/dev/dvb/adapter%d/frontend%d", adapter_num, frontend_num);

 					fe_fd = file_open(devstr, O_RDONLY, bytes);

 					if (fe_fd != NULL)
 					{
 						struct dvb_device *dvbdev = fe_fd->private_data;
 						struct dvb_frontend_info fe_info;
 						struct dtv_property p[] = {{ .cmd = DTV_DELIVERY_SYSTEM }};
 						struct dtv_properties cmdseq = { .num = 1, .props = p };

 						if (dvb_generic_ioctl(fe_fd, FE_GET_INFO, 0))
 						{
 							dvbdev->kernel_ioctl(fe_fd, FE_GET_INFO, &fe_info);

 							if (dvb_generic_ioctl(fe_fd, FE_GET_PROPERTY, 0))
 							{
 								dvbdev->kernel_ioctl(fe_fd, FE_GET_PROPERTY, &cmdseq);
 	 						

/* 	2nd generation DVB Tuner detected adding 2 to the TunerType */
								if ( (fe_info.caps & FE_CAN_2G_MODULATION ) == FE_CAN_2G_MODULATION )
								{
 									seq_printf(m,
 									"NIM Socket %d:\n"
 									"\tType: %s2\n"
 									"\tName: %s\n"
 									"\tHas_Outputs: no\n"
 								//	"\tInternally_Connectable: 0\n"
 									"\tFrontend_Device: %d\n"
 								//	"\tI2C_Device: -1\n"
									,
									nsocket_index,
		 							DVBTunerType[fe_info.type],
 									fe_info.name,
 									frontend_num
 									);
		 						}	
								else	{
									seq_printf(m,
	 								"NIM Socket %d:\n"
 									"\tType: %s\n"
	 								"\tName: %s\n"
 									"\tHas_Outputs: no\n"
 								//	"\tInternally_Connectable: 0\n"
 									"\tFrontend_Device: %d\n"
 								//	"\tI2C_Device: -1\n"
 									,
	 								nsocket_index,
 									DVBTunerType[fe_info.type],
 									fe_info.name,
 									frontend_num
 									);
								}
	
								nsocket_index++; 					
 							}
 						}
					}

 					frontend_num++;

 					file_close(fe_fd);
 				}

 				adapter_num++;
 			}
		}
	}

	return 0;
}
