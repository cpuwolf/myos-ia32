struct driver_struct
{
	int (*open)(struct driver_struct * dv);
	int (*read)(struct driver_struct * dv,unsigned char * buf,int size,int count);
	int (*write)(struct driver_struct * dv,unsigned char *buf,int size,int count);
	int (*close)(struct driver_struct * dv);
};
