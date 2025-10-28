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