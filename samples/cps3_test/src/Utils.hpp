#pragma once

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include "cps/CPS_API.hpp"

#include <algorithm>
#include <iostream>
#include <string>

inline std::ostream& print_error(std::ostream& os, const CPS::Error& error)
{
  return os << "ERROR " << error.code << " - " << error.text << ": " << CPS::Utils::join(error.documentIds) << std::endl;
}

inline std::ostream& print_errors(std::ostream& os, const std::vector<CPS::Error>& error)
{
  std::for_each(error.begin(), error.end(), [&os](const CPS::Error& err)
  {
    print_error(os, err);
  });
  return os;
}

inline std::string make_docid(const std::string& prefix, int id)
{
  std::string docid(prefix);
  docid += std::to_string(id);
  return docid;
}

inline std::string get_docid(CPS::XMLDocument* xmldoc)
{
  auto nodes = xmldoc->FindFast("/document/id", false);
  if (nodes.empty())
  {
    throw std::runtime_error("Document without Id");
  }
  return nodes.front()->getContent();
}

#endif /* UTILS_HPP_ */
