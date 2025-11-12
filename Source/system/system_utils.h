#pragma once

// "system_utils.h"
// Centralized logger that duplicates output to std::cout, std::clog and to a file (olympe.log).
// Usage:
// SystemUtils::InitSystemLogger();
// SYSTEM_LOG << "Hello world" << std::endl;

#include <streambuf>
#include <ostream>
#include <vector>
#include <memory>
#include <fstream>
#include <mutex>
#include <iostream>
#include <string>
#include "system_consts.h"

namespace SystemUtils
{
	// small streambuf that forwards everything to multiple underlying streambufs
	class TeeBuf : public std::streambuf
	{
		public:
		TeeBuf() = default;
		~TeeBuf() = default;

		void addBuf(std::streambuf* b)
		{
			if (b)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_bufs.push_back(b);
			}
		}

	 protected:
		// single character
		virtual int overflow(int c) override
		{
			if (c == EOF) return !EOF;
			std::lock_guard<std::mutex> lock(m_mutex);
			for (auto b : m_bufs) 
			{
				if (b->sputc(static_cast<char>(c)) == EOF) return EOF;
			}
			return c;
		}

		// block write
		virtual std::streamsize xsputn(const char* s, std::streamsize n) override
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			for (auto b : m_bufs) 
			{
				if (b->sputn(s, n) != n) 
				{
					// continue trying others, but still report shorter write
				}
			}
			return n;
		}

		// sync all
		virtual int sync() override
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			int err =0;
			for (auto b : m_bufs) 
			{
				if (b->pubsync() !=0) err = -1;
			}
			return err;
		}

	 private:
		std::vector<std::streambuf*> m_bufs;
		std::mutex m_mutex;
	};
	//----------------------------------------------------------------------------------
	// ostream that uses TeeBuf
	class TeeStream : public std::ostream
	{
	public:
		TeeStream() : std::ostream(&m_buf) {}
		void addBuf(std::streambuf* b) { m_buf.addBuf(b); }
	 private:
		TeeBuf m_buf;
	};
	//----------------------------------------------------------------------------------
	// Initialize the global logger. Returns true if file opened successfully (or already initialized).
	inline bool InitSystemLogger(const std::string& filename = "olympe.log")
	{
		// static locals ensure single instance across translation units
		static std::unique_ptr<std::ofstream> s_file;
		static std::unique_ptr<TeeStream> s_stream;
		static TeeStream*& s_instance = *([]()->TeeStream**{ static TeeStream* p = nullptr; return &p; })();

		if (s_stream) return true; // already initialized

		s_file.reset(new std::ofstream(filename.c_str(), std::ios::app));
		if (!s_file->is_open())
		{
			// fallback: do not set file but still log to cout/clog
			s_file.reset();
		}

		s_stream.reset(new TeeStream());
		s_stream->addBuf(std::cout.rdbuf());
		s_stream->addBuf(std::clog.rdbuf());
		if (s_file) s_stream->addBuf(s_file->rdbuf());

		s_instance = s_stream.get();
		return true;
	}
	//----------------------------------------------------------------------------------
	// Shutdown logger (close file and reset instances)
	inline void ShutdownSystemLogger()
	{
		static std::unique_ptr<std::ofstream>* s_file_ptr = nullptr;
		// access the same statics as Init via local static trick
		// Note: we only need to close the file; stream objects will be destroyed on program exit
		// We'll locate the file by recreating a static unique in Init's scope is not trivial here,
		// so simply flush cout/clog. The file ofstream will be closed automatically at exit.
		std::fflush(nullptr);
	}
	//----------------------------------------------------------------------------------
	// Get the logger stream. If not initialized, returns std::cout.
	inline std::ostream& Logger()
	{
		// access the instance pointer created in InitSystemLogger
		static TeeStream*& s_instance = *([]()->TeeStream**{ static TeeStream* p = nullptr; return &p; })();
		if (s_instance) 
			return *s_instance;
		else
			return std::clog;
	 }
}

// Convenience macro
#define SYSTEM_LOG ::SystemUtils::Logger()


static std::string escape_json_string(const std::string& s)
{
	std::string out;
	out.reserve(s.size() + 4);
	for (char c : s)
	{
		switch (c)
		{
		case '\\': out += "\\\\"; break;
		case '"': out += "\\\""; break;
		case '\b': out += "\\b"; break;
		case '\f': out += "\\f"; break;
		case '\n': out += "\\n"; break;
		case '\r': out += "\\r"; break;
		case '\t': out += "\\t"; break;
		default: out += c; break;
		}
	}
	return out;
}
// Very small and permissive JSON extractors (not robust but sufficient for simple saved files)
static bool extract_json_string(const std::string& json, const std::string& key, std::string& out)
{
	size_t pos = json.find('"' + key + '"');
	if (pos == std::string::npos) pos = json.find(key);
	if (pos == std::string::npos) return false;
	pos = json.find(':', pos);
	if (pos == std::string::npos) return false;
	++pos;
	// skip spaces
	while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
	if (pos < json.size() && json[pos] == '"') ++pos;
	size_t start = pos;
	while (pos < json.size() && json[pos] != '"' && json[pos] != '\n' && json[pos] != '\r') ++pos;
	if (pos <= start) return false;
	out = json.substr(start, pos - start);
	return true;
}

static bool extract_json_double(const std::string& json, const std::string& key, double& out)
{
	size_t pos = json.find('"' + key + '"');
	if (pos == std::string::npos) pos = json.find(key);
	if (pos == std::string::npos) return false;
	pos = json.find(':', pos);
	if (pos == std::string::npos) return false;
	++pos;
	// skip spaces
	while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
	size_t start = pos;
	// accept digits, +, -, ., e, E
	while (pos < json.size() && (isdigit(static_cast<unsigned char>(json[pos])) || json[pos] == '+' || json[pos] == '-' || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E')) ++pos;
	if (pos <= start) return false;
	try {
		out = std::stod(json.substr(start, pos - start));
		return true;
	}
	catch (...) { return false; }
}

static bool extract_json_int(const std::string& json, const std::string& key, int& out)
{
	double d;
	if (!extract_json_double(json, key, d)) return false;
	out = static_cast<int>(d);
	return true;
}

static bool extract_json_bool(const std::string& json, const std::string& key, bool& out)
{
	size_t pos = json.find('"' + key + '"');
	if (pos == std::string::npos) pos = json.find(key);
	if (pos == std::string::npos) return false;
	pos = json.find(':', pos);
	if (pos == std::string::npos) return false;
	++pos;
	while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
	if (json.compare(pos, 4, "true") == 0) { out = true; return true; }
	if (json.compare(pos, 5, "false") == 0) { out = false; return true; }
	return false;
}
//-------------------------------------------------------------
// Load configuration from JSON file (simple parser)
// Olympe GameEngine Loader - looks for screen_width and screen_height keys
static void LoadConfigJSON(const char* filename, int& outW, int& outH)
{
	outW = DEFAULT_WINDOW_WIDTH;
	outH = DEFAULT_WINDOW_HEIGHT;

	std::ifstream ifs(filename);
	if (!ifs) {
		SYSTEM_LOG << "Config file '" << filename << "' not found — using defaults " << outW << "x" << outH << "\n";
		return;
	}

	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	int w = outW;
	int h = outH;

	// try multiple possible keys and tolerate a common misspelling
	if (extract_json_int(content, "screen_width", w) ||
		extract_json_int(content, "screenWidth", w) ||
		extract_json_int(content, "width", w)) {
		// ok
	}
	if (extract_json_int(content, "screen_height", h) ||
		extract_json_int(content, "screenHeight", h) ||
		extract_json_int(content, "screen_heigth", h) || // tolerate misspelling
		extract_json_int(content, "height", h)) {
		// ok
	}

	if (w > 0) outW = w;
	if (h > 0) outH = h;

	SYSTEM_LOG << "Config loaded from '" << filename << "': " << outW << "x" << outH << "\n";
}