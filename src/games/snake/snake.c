int* main(void) {
	char str1[50] = "HELLO FROM \n ELF!";
	test(str1);

	return -11;
}

void test(char* data) {
	volatile char *video = (volatile char*)0xB8000;
    while( *data != 0 ) {
        *video++ = *data++;
        *video++ = 0x07;
    }
}