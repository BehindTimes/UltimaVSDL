#pragma once
#include <string>
#include <vector>

struct dict_entry
{
	unsigned char root;
	int codeword;
};

class LzwDecompressor
{
public:
	static bool Extract(std::vector<unsigned char>& buffer, std::vector<unsigned char>& out_data);
private:
	static const int MyStack_size = 10000;
	static const int dict_size = 10000;
	static const int max_codeword_length = 12;

	static bool Is_valid_lzw_file(std::vector<unsigned char>& buffer);
	static uint32_t read4(std::vector<unsigned char>& file_bytes);
	static bool Get_next_codeword_u4(int& codeword, std::vector<unsigned char>& source);
	static bool Lzw_decompressbuffer(std::vector<unsigned char>& source, std::vector<unsigned char>& destination);
	static bool get_next_codeword(int& codeword, std::vector<unsigned char>& source, int codeword_size);
	static void Dict_init();
	static void Stack_init();
	static void Output_root(unsigned char root, std::vector<unsigned char>& destination);
	static unsigned char Dict_get_root(int codeword);
	static int Dict_get_codeword(int codeword);
	static bool Stack_is_empty();
	static bool Stack_is_full();
	static void Stack_push(unsigned char element);
	static unsigned char Stack_pop();
	static unsigned char Stack_gettop();
	static void Dict_add(unsigned char root, int codeword);
	static void Get_string(int codeword);

	static int bits_read;
	static int dict_contains;
	static int stack_contains;
	static size_t position;
	static std::vector<dict_entry> dict;
	static std::vector<unsigned char> mystack;
};

