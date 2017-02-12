#include "ConfigParser.h"

ConfigParser::ConfigParser()
	: position{}
{}

void ConfigParser::parse(std::string const& filename, std::istream& is) {
	std::string line;
	int line_no = 0;
	while (std::getline(is, line))
		parseLine(std::move(line), filename, line_no++);
}

bool ConfigParser::atHeader() const {
	return !atEnd() && lines[position].is_header;
}

bool ConfigParser::atKVP() const {
	return !atEnd() && !lines[position].is_header;
}

bool ConfigParser::atEnd() const {
	return position == lines.size();
}

std::string ConfigParser::getHeader() {
	if (!atHeader())
		error("header expected");
	return lines[position++].data;
}

std::pair<std::string, std::string> ConfigParser::getKVP() {
	if (!atKVP())
		error("KVP expected");
	auto result = std::make_pair(lines[position].data, lines[position].extra_data);
	position += 1;
	return result;
}

void ConfigParser::error(std::string str) {
	if (atEnd()) {
		if (position != 0)
			--position;
		throw ParseError(std::move(str), lines[position].filename, -1);
	} else {
		throw ParseError(std::move(str), lines[position].filename, lines[position].line_no);
	}
}

void ConfigParser::parseLine(std::string line, std::string const& filename, int line_no) {
	// maybe check for all-whitespace lines, too?
	if (line.empty() || line.find('#') != std::string::npos)
		return;
	Line result;
	result.line_no = line_no;
	result.filename = filename;
	auto lbrace = line.find('[');
	if (lbrace != std::string::npos) {
		auto rbrace = line.find(']');
		if (rbrace == std::string::npos || rbrace < lbrace)
			error("unmatched [");
		result.data = line.substr(lbrace+1, rbrace-lbrace-1);
		result.is_header = true;
		lines.push_back(result);
		return;
	}
	auto eq = line.find('=');
	if (eq == std::string::npos)
		error("unrecognised data");
	result.data = line.substr(0, eq);
	result.extra_data = line.substr(eq+1);
	lines.push_back(result);
}

char const* ParseError::what() const noexcept {
	if (!m_err_msg.empty())
		return m_err_msg.c_str();
	m_err_msg += "in ";
	m_err_msg += m_file;
	m_err_msg += ", at ";
	if (m_line != -1)
		m_err_msg += "line " + std::to_string(m_line);
	else
		m_err_msg += "EOF";
	m_err_msg += ": ";
	m_err_msg += std::runtime_error::what();
	return m_err_msg.c_str();
}
