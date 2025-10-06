#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DEVISIZE 8192

inline int chk_env_size(const char* env_name, const char* env, const size_t stdsize){
	// env should not never be empty
	size_t size = strlen(env);
	if(size != stdsize){
		fprintf(stderr, "Error, the size of %s should be %zu but got %zu\n", env_name, stdsize, size);
		return 1;
	}
	return 0;
}

static inline int set_new_value(FILE* fp, const char* var, const char* newvalue, const size_t size){
	static unsigned char buffer[DEVISIZE] = {};
	static bool read = false;
	rewind(fp);
	if(!read){
		if(fread(buffer, 1, DEVISIZE, fp) != DEVISIZE){
			perror("Error reading file");
			return 1;
		}
		read = true;
	}
	unsigned char* pos = memmem(buffer, DEVISIZE, var, strlen(var));
	if(!pos){
		fprintf(stderr, "Pattern %s not found in file\n", var);
		return 1;
	}

	size_t offset = (pos - buffer) + strlen(var) + 1;
	if (fseek(fp, offset, SEEK_SET) != 0){
		perror("Error seeking offset");
		return 1;
	}

	size_t written = fwrite(newvalue, 1, size, fp);
	if(written != size){
		char err_msg[64];
		sprintf(err_msg, "Error writing new value for %s", var);
		perror(err_msg);
		return 1;
	}
	return 0;
}

static inline int valid_set_new_value(FILE* fp, const char* var, const char* newvalue, const size_t size){
	rewind(fp);
	if (newvalue && !chk_env_size(var, newvalue, size)){
		int status = set_new_value(fp, var, newvalue, size);
		if (status){
			fclose(fp);
			return 1;
		}
	}
	return 0;
}

int main(){
	const char *imgfilename = "devinfo.img";
	FILE *fp = fopen(imgfilename, "r+b");
	if (!fp){
		perror("Error opening devinfo.img file");
		return 1;
	}

	const char *sku_name = "sku";
	const char *dsn_name = "sn"; // Serial number displayed on phone, dsn
	const char *wlan_mac_name = "wlanmac"; // wlan_mac1
	const char *bt_addr_name = "btaddr"; // bt_addr
	const char *imei1_name = "imei1";
	const char *imei2_name = "imei2";

	const size_t sku_length = 5;
	const size_t dsn_length = 14;
	const size_t mac_length = 17;
	const size_t imei_length = 15;
	/* Do we need to add other things like psn, pcbcfg, devcfg, etc.? */

	const char* sku = getenv(sku_name);
	const char* dsn = getenv(dsn_name);
	const char* wlan_mac = getenv(wlan_mac_name);
	const char* bt_addr = getenv(bt_addr_name);
	const char* imei1 = getenv(imei1_name);
	const char* imei2 = getenv(imei2_name);

	if(valid_set_new_value(fp, sku_name, sku, sku_length)) return 1;
	if(valid_set_new_value(fp, dsn_name, dsn, dsn_length)) return 1;
	if(valid_set_new_value(fp, wlan_mac_name, wlan_mac, mac_length)) return 1;
	if(valid_set_new_value(fp, bt_addr_name, bt_addr, mac_length)) return 1;
	if(valid_set_new_value(fp, imei1_name, imei1, imei_length)) return 1;
	if(valid_set_new_value(fp, imei2_name, imei2, imei_length)) return 1;

	fclose(fp);
	return 0;
}
