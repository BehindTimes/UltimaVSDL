#include "LzwDecompressor.h"

int LzwDecompressor::bits_read = 0;
int LzwDecompressor::dict_contains = 0;
int LzwDecompressor::stack_contains = 0;
size_t LzwDecompressor::position = 0;
std::vector<dict_entry> LzwDecompressor::dict(dict_size);
std::vector<unsigned char> LzwDecompressor::mystack(MyStack_size);

bool LzwDecompressor::Extract(std::vector<unsigned char>& buffer, std::vector<unsigned char>& out_data)
{
    bits_read = 0;
    dict_contains = 0;
    stack_contains = 0;
    position = 0;

	if (Is_valid_lzw_file(buffer))
	{
        // determine the buffer sizes
        //size_t source_buffer_size = buffer.size() - 4;
        int destination_buffer_size = read4(buffer);

        // create the buffers
        std::vector<unsigned char> source_buffer;
        std::vector<unsigned char> destination_buffer(destination_buffer_size);

		source_buffer.insert(source_buffer.begin(), buffer.begin() + 4, buffer.end());

        // decompress the input file
        if (!Lzw_decompressbuffer(source_buffer, destination_buffer))
        {
            return false;
        }
        out_data = destination_buffer;
	}
	return true;
}

uint32_t LzwDecompressor::read4(std::vector<unsigned char>& file_bytes)
{
    unsigned char b0, b1, b2, b3;
    b0 = file_bytes[0];
    b1 = file_bytes[1];
    b2 = file_bytes[2];
    b3 = file_bytes[3];
    return (b0 + (b1 << 8) + (b2 << 16) + (b3 << 24));
}

// this function only checks a few *necessary* conditions
// returns "false" if the file doesn't satisfy these conditions
// return "true" otherwise
bool LzwDecompressor::Is_valid_lzw_file(std::vector<unsigned char>& file_bytes)
{
    // file must contain 4-byte size header and space for the 9-bit value 0x100
    if (file_bytes.size() < 6)
    {
        return false;
    }
    // the last byte of the size header must be 0 (U5's files aren't *that* big)
    if (file_bytes[3] != 0)
    {
        return false;
    }
    // the 9 bits after the size header must be 0x100
    if ((file_bytes[4] != 0) || ((file_bytes[5] & 1) != 1))
    {
        return false;
    }

    return true;
}

// ----------------------------------------------
// Read the next code word from the source buffer
// ----------------------------------------------
bool LzwDecompressor::get_next_codeword(int& codeword, std::vector<unsigned char> &source, int codeword_size)
{
	unsigned char b0 = 0;
	unsigned char b1 = 0;
	unsigned char b2 = 0;

	int xx, xy, xz;
	xx = bits_read / 8;
	if (bits_read / 8 >= 0x1520)
	{
		return false;
	}
	b0 = source[bits_read / 8];
	if (source.size() > bits_read / 8 + 1)
	{
		b1 = source[bits_read / 8 + 1];
	}
	if (source.size() > bits_read / 8 + 2)
	{
		b2 = source[bits_read / 8 + 2];
	}
	xx = b2 << 16;
	xy = b1 << 8;
	xz = b0;
	codeword = xx + xy + xz;
	codeword = codeword >> (bits_read % 8);
	switch (codeword_size)
	{
	case 0x9:
		codeword = codeword & 0x1ff;
		break;
	case 0xa:
		codeword = codeword & 0x3ff;
		break;
	case 0xb:
		codeword = codeword & 0x7ff;
		break;
	case 0xc:
		codeword = codeword & 0xfff;
		break;
	default:
		printf("Error: weird codeword size!\n");
		return false;
	}
	bits_read += codeword_size;

	return true;
}

bool LzwDecompressor::Get_next_codeword_u4(int &codeword, std::vector<unsigned char>& source)
{
	codeword = (source[bits_read / 8] << 8) + source[bits_read / 8 + 1];
	codeword = codeword >> (4 - (bits_read % 8));
	codeword = codeword & 0xfff;

	bits_read += 12;

	return true;
}

void LzwDecompressor::Dict_init()
{
	dict_contains = 0x102;
}

void LzwDecompressor::Stack_init()
{
	stack_contains = 0;
}

void LzwDecompressor::Output_root(unsigned char root, std::vector<unsigned char>& destination)
{
	destination[position] = root;
	position++;
}

unsigned char LzwDecompressor::Dict_get_root(int codeword)
{
	return (dict[codeword].root);
}

int LzwDecompressor::Dict_get_codeword(int codeword)
{
	return (dict[codeword].codeword);
}

bool LzwDecompressor::Stack_is_empty()
{
	return (stack_contains == 0);
}

bool LzwDecompressor::Stack_is_full()
{
	return (stack_contains == mystack.size());
}

void LzwDecompressor::Stack_push(unsigned char element)
{
	if (!Stack_is_full())
	{
		mystack[stack_contains] = element;
		stack_contains++;
	}
}

unsigned char LzwDecompressor::Stack_pop()
{
	unsigned char element;

	if (!Stack_is_empty())
	{
		element = mystack[stack_contains - 1];
		stack_contains--;
	}
	else
	{
		element = 0;
	}
	return element;
}

unsigned char LzwDecompressor::Stack_gettop()
{
	if (!Stack_is_empty())
	{
		return (mystack[stack_contains - 1]);
	}
	return 0;
}

void LzwDecompressor::Dict_add(unsigned char root, int codeword)
{
	dict[dict_contains].root = root;
	dict[dict_contains].codeword = codeword;
	dict_contains++;
}

void LzwDecompressor::Get_string(int codeword)
{
	unsigned char root;
	int current_codeword;

	current_codeword = codeword;
	Stack_init();
	while (current_codeword > 0xff)
	{
		root = Dict_get_root(current_codeword);
		current_codeword = Dict_get_codeword(current_codeword);
		Stack_push(root);
	}

	// push the root at the leaf
	Stack_push((unsigned char)current_codeword);
}

// -----------------------------------------------------------------------------
// LZW-decompress from buffer to buffer.
// -----------------------------------------------------------------------------
bool LzwDecompressor::Lzw_decompressbuffer(std::vector<unsigned char> &source, std::vector<unsigned char>& destination)
{
    bool end_marker_reached = false;
    int codeword_size = 9;
    int next_free_codeword = 0x102;
    int dictionary_size = 0x200;

    size_t bytes_written = 0;

    int cW;
    int pW = 0;
    unsigned char C;
    int poppedval;

	while (!end_marker_reached)
	{
		bool bValid = get_next_codeword(cW, source, codeword_size);
		if (!bValid)
		{
			return false;
		}

		switch (cW)
		{
			// re-init the dictionary
		case 0x100:
			codeword_size = 9;
			next_free_codeword = 0x102;
			dictionary_size = 0x200;
			Dict_init();
			bValid = get_next_codeword(cW, source, codeword_size);
			if (!bValid)
			{
				return false;
			}

			position = bytes_written;
			Output_root((unsigned char)cW, destination);
			bytes_written = position;
			break;
			// end of compressed file has been reached
		case 0x101:
			end_marker_reached = 1;
			break;
			// (cW <> 0x100) && (cW <> 0x101)
		default:
			if (cW < next_free_codeword)
			{  // codeword is already in the dictionary
				// create the string associated with cW (on the MyStack)
				Get_string(cW);
				C = Stack_gettop();
				// output the string represented by cW

				while (!Stack_is_empty())
				{
					poppedval = Stack_pop();
					position = bytes_written;
					Output_root((unsigned char)poppedval, destination);
					bytes_written = position;
				}

				// add pW+C to the dictionary
				Dict_add(C, pW);

				next_free_codeword++;
				if (next_free_codeword >= dictionary_size)
				{
					if (codeword_size < max_codeword_length)
					{
						codeword_size += 1;
						dictionary_size *= 2;
					}
				}
			}
			else
			{  // codeword is not yet defined
				// create the string associated with pW (on the MyStack)
				Get_string(pW);
				C = Stack_gettop();
				// output the string represented by pW
				while (!Stack_is_empty())
				{
					position = bytes_written;
					Output_root(Stack_pop(), destination);
					bytes_written = position;
				}
				// output the char C
				position = bytes_written;
				Output_root(C, destination);
				bytes_written = position;
				// the new dictionary entry must correspond to cW
				// if it doesn't, something is wrong with the lzw-compressed data.
				if (cW != next_free_codeword)
				{
					printf("cW != next_free_codeword!\n");
					return false; // ERROR!  Come back later and close
				}
				// add pW+C to the dictionary
				Dict_add(C, pW);

				next_free_codeword++;
				if (next_free_codeword >= dictionary_size)
				{
					if (codeword_size < max_codeword_length)
					{
						codeword_size += 1;
						dictionary_size *= 2;
					}
				}
			};
			break;
		}
		// shift roles - the current cW becomes the new pW
		pW = cW;
	}

    return true;
}
