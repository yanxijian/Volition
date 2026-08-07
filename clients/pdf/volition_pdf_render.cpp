// Out-of-process pdfium renderer — must NOT load MPS/protobuf (abseil clash).
// Usage:
//   volition_pdf_render --file doc.pdf --info
//   volition_pdf_render --file doc.pdf --page 0 --zoom 1.25 --out page.bmp

#include "public/fpdfview.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace
{
	struct Args
	{
		std::string file;
		std::string out;
		int page = 0;
		double zoom = 1.25;
		bool info = false;
	};

	bool parseArgs(int argc, char** argv, Args& out)
	{
		for (int i = 1; i < argc; ++i)
		{
			const char* a = argv[i];
			auto take = [&](std::string& dest) -> bool
			{
				if (i + 1 >= argc)
				{
					return false;
				}
				dest = argv[++i];
				return true;
			};
			if (std::strcmp(a, "--file") == 0)
			{
				if (!take(out.file))
					return false;
			}
			else if (std::strcmp(a, "--out") == 0)
			{
				if (!take(out.out))
					return false;
			}
			else if (std::strcmp(a, "--page") == 0)
			{
				std::string v;
				if (!take(v))
					return false;
				out.page = std::atoi(v.c_str());
			}
			else if (std::strcmp(a, "--zoom") == 0)
			{
				std::string v;
				if (!take(v))
					return false;
				out.zoom = std::atof(v.c_str());
			}
			else if (std::strcmp(a, "--info") == 0)
			{
				out.info = true;
			}
			else
			{
				return false;
			}
		}
		return !out.file.empty() && (out.info || !out.out.empty());
	}

	bool writeBmpBgra(const char* path, int width, int height, const unsigned char* bgra)
	{
		// Bottom-up BMP, 32bpp BGRA as stored by pdfium.
		const int rowBytes = width * 4;
		const int fileSize = 14 + 40 + rowBytes * height;
		std::vector<unsigned char> hdr(14 + 40, 0);
		hdr[0] = 'B';
		hdr[1] = 'M';
		hdr[2] = static_cast<unsigned char>(fileSize);
		hdr[3] = static_cast<unsigned char>(fileSize >> 8);
		hdr[4] = static_cast<unsigned char>(fileSize >> 16);
		hdr[5] = static_cast<unsigned char>(fileSize >> 24);
		hdr[10] = 54;
		hdr[14] = 40;
		hdr[18] = static_cast<unsigned char>(width);
		hdr[19] = static_cast<unsigned char>(width >> 8);
		hdr[20] = static_cast<unsigned char>(width >> 16);
		hdr[21] = static_cast<unsigned char>(width >> 24);
		hdr[22] = static_cast<unsigned char>(height);
		hdr[23] = static_cast<unsigned char>(height >> 8);
		hdr[24] = static_cast<unsigned char>(height >> 16);
		hdr[25] = static_cast<unsigned char>(height >> 24);
		hdr[26] = 1;
		hdr[28] = 32;

		FILE* f = nullptr;
#ifdef _WIN32
		fopen_s(&f, path, "wb");
#else
		f = std::fopen(path, "wb");
#endif
		if (!f)
		{
			return false;
		}
		std::fwrite(hdr.data(), 1, hdr.size(), f);
		for (int y = height - 1; y >= 0; --y)
		{
			std::fwrite(bgra + static_cast<size_t>(y) * rowBytes, 1, static_cast<size_t>(rowBytes), f);
		}
		std::fclose(f);
		return true;
	}
} // namespace

int main(int argc, char** argv)
{
	std::vector<std::string> storage;
	std::vector<char*> utf8Argv;
#ifdef _WIN32
	int wargc = 0;
	LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
	if (!wargv)
	{
		std::fprintf(stderr, "CommandLineToArgvW failed\n");
		return 1;
	}
	storage.reserve(static_cast<size_t>(wargc));
	utf8Argv.reserve(static_cast<size_t>(wargc));
	for (int i = 0; i < wargc; ++i)
	{
		const int n = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
		std::string s(static_cast<size_t>(n > 0 ? n - 1 : 0), '\0');
		if (n > 1)
		{
			WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, s.data(), n, nullptr, nullptr);
		}
		storage.push_back(std::move(s));
		utf8Argv.push_back(storage.back().data());
	}
	LocalFree(wargv);
	argc = wargc;
	argv = utf8Argv.data();
#endif

	Args args;
	if (!parseArgs(argc, argv, args))
	{
		std::fprintf(stderr, "usage: volition_pdf_render --file PATH (--info | --page N --zoom F --out BMP)\n");
		return 2;
	}

	FPDF_InitLibrary();
	FPDF_DOCUMENT doc = FPDF_LoadDocument(args.file.c_str(), nullptr);
	if (!doc)
	{
		std::fprintf(stderr, "FPDF_LoadDocument failed\n");
		FPDF_DestroyLibrary();
		return 3;
	}

	const int pageCount = FPDF_GetPageCount(doc);
	if (args.info)
	{
		std::printf("pages=%d\n", pageCount);
		FPDF_CloseDocument(doc);
		FPDF_DestroyLibrary();
		return 0;
	}

	if (args.page < 0 || args.page >= pageCount)
	{
		std::fprintf(stderr, "page out of range\n");
		FPDF_CloseDocument(doc);
		FPDF_DestroyLibrary();
		return 4;
	}

	FPDF_PAGE page = FPDF_LoadPage(doc, args.page);
	if (!page)
	{
		std::fprintf(stderr, "FPDF_LoadPage failed\n");
		FPDF_CloseDocument(doc);
		FPDF_DestroyLibrary();
		return 5;
	}

	const double pageW = FPDF_GetPageWidth(page);
	const double pageH = FPDF_GetPageHeight(page);
	const int width = pageW * args.zoom > 1.0 ? static_cast<int>(pageW * args.zoom + 0.5) : 1;
	const int height = pageH * args.zoom > 1.0 ? static_cast<int>(pageH * args.zoom + 0.5) : 1;
	std::vector<unsigned char> buffer(static_cast<size_t>(width) * static_cast<size_t>(height) * 4u, 255);
	FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(width, height, FPDFBitmap_BGRA, buffer.data(), width * 4);
	if (!bitmap)
	{
		std::fprintf(stderr, "FPDFBitmap_CreateEx failed\n");
		FPDF_ClosePage(page);
		FPDF_CloseDocument(doc);
		FPDF_DestroyLibrary();
		return 6;
	}
	FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, FPDF_ANNOT);
	FPDFBitmap_Destroy(bitmap);
	FPDF_ClosePage(page);
	FPDF_CloseDocument(doc);
	FPDF_DestroyLibrary();

	if (!writeBmpBgra(args.out.c_str(), width, height, buffer.data()))
	{
		std::fprintf(stderr, "write bmp failed\n");
		return 7;
	}
	return 0;
}
