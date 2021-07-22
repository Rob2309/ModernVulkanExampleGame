#pragma once

#include <source_location>
#include <iostream>
#include <format>

namespace Log {

#ifdef NDEBUG
	inline constinit auto ENABLE_INFO_LOG = false;
#else
	inline constinit auto ENABLE_INFO_LOG = true;
#endif

	/// <summary>
	/// Concept that enforces that an object can be formatted.
	/// </summary>
	template<typename T>
	concept Printable = requires(const T& obj) {
		{ std::format("{}", obj) } -> std::convertible_to<std::string>;
	};

	/// <summary>
	/// Helper struct that captures the std::source_location of a log message, as this does not work with template parameter packs.
	/// </summary>
	struct FormatWithSourceLoc {
		const char* fmt;
		const std::source_location loc;

		constexpr FormatWithSourceLoc(const char* fmt, std::source_location loc = std::source_location::current())
			: fmt{fmt}, loc{loc}
		{ }
	};

	/// <summary>
	/// Log an Info Message
	/// </summary>
	/// <param name="fmt">The format string. Has to be consteval</param>
	/// <param name="...args">Formatting arguments</param>
	template<Printable... Args>
	void Info(const FormatWithSourceLoc fmt, Args&&... args) {
		if /*consteval*/ (ENABLE_INFO_LOG) {
			auto msg = std::format(fmt.fmt, args...);
			std::cout << "[INFO ] " << msg << "\n    at " << fmt.loc.file_name() << ":" << fmt.loc.line() << "\n";
		}
	}

	/// <summary>
	/// Log a Warning Message
	/// </summary>
	/// <param name="fmt">The format string. Has to be consteval</param>
	/// <param name="...args">Formatting arguments</param>
	template<Printable... Args>
	void Warning(const FormatWithSourceLoc fmt, Args&&... args) {
		auto msg = std::format(fmt.fmt, args...);
		std::cout << "\033[33m[WARN ] " << msg << "\n    at " << fmt.loc.file_name() << ":" << fmt.loc.line() << "\n\033[0m";
	}

	/// <summary>
	/// Log an Error Message
	/// </summary>
	/// <param name="fmt">The format string. Has to be consteval</param>
	/// <param name="...args">Formatting arguments</param>
	template<Printable... Args>
	void Error(const FormatWithSourceLoc fmt, Args&&... args) {
		auto msg = std::format(fmt.fmt, args...);
		std::cout << "\033[31m[ERROR] " << msg << "\n    at " << fmt.loc.file_name() << ":" << fmt.loc.line() << "\n\033[0m";
	}
	
}
