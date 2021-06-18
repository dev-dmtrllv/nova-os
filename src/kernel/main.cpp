extern "C" void kmain() __attribute__((section(".text.main")));

void write_string(const char *string);
void write_char(const char c);

extern "C" void kmain()
{
	write_string("Hello, World!");
	write_char('B');
	while(true){}
}


void write_string(const char *string)
{
    volatile char *video = (volatile char*)0xB8000;
    while(*string != '\0')
    {
        *video++ = *string++;
        *video++ = 0xf0;
    }
}

void write_char(const char c)
{
    volatile char *video = (volatile char*)0xB8000;
    *video++ = c;
}
