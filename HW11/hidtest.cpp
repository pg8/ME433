#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	char cbuf[65];
	unsigned char message[25];
	unsigned char row;
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i = 0;
	int j = 0;
	int check;
	unsigned char charCurrent=1;

FILE *ofp;
	ofp = fopen("accels.txt", "w");


	unsigned char temp;
	int xaccel;
	int yaccel;
	int zaccel;
	unsigned char temp2;
	printf("this works\n");

	printf("Message: ");
	scanf(" %25[^\n]",&message);
	printf("Row number: ");
	scanf(" %c",&row);
	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);

	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	buf[2] = row-0x30;
	i=3;
	while(charCurrent!=0){
		charCurrent=message[i-3];
		buf[i]=charCurrent;
		i+=1;

	}
	i-=1;
	while(i<=27){
		buf[i]=' ';
		i+=1;
	}
	res = hid_write(handle, buf, 65);
j=0;
	// Request state (cmd 0x81). The first byte is the report number (0x0).
while(j<1000){
	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);
	// Read requested state

	res = hid_read(handle, buf, 65);
	if(buf[0]){


for(i=0;i<65;i++){
	cbuf[i]=buf[i];
}

	sscanf(cbuf,"%d %d %d",&xaccel,&yaccel,&zaccel);

	//printf("xaccel: %d\t yaccel: %d\t zaccel: %d\r      ",xaccel,yaccel,zaccel);







fprintf(ofp,"%d\n",zaccel);
j++;
}


}
	// Finalize the hidapi library
	res = hid_exit();
fclose(ofp);

	return 0;

}

