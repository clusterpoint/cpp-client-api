#ifndef CPS_API_HPP
#define CPS_API_HPP

#ifndef VERSION
#define VERSION "1.0.0"
#endif // VERSION

#include "Exception.hpp"
#include "Connection.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Xmldocument.hpp"
#include "Utils.hpp"

// Request headers
#include "requests/AlternativesRequest.hpp"
#include "requests/ListFacetsRequest.hpp"
#include "requests/ListLastRetrieveFirstRequest.hpp"
#include "requests/ListPathsRequest.hpp"
#include "requests/ListWordsRequest.hpp"
#include "requests/LookupRequest.hpp"
#include "requests/ModifyRequest.hpp"
#include "requests/RetrieveRequest.hpp"
#include "requests/SearchDeleteRequest.hpp"
#include "requests/SearchRequest.hpp"
#include "requests/ShowHistoryRequest.hpp"
#include "requests/SimilarRequest.hpp"
#include "requests/StatusRequest.hpp"

// Response headers
#include "responses/AlternativesResponse.hpp"
#include "responses/ListFacetsResponse.hpp"
#include "responses/ListPathsResponse.hpp"
#include "responses/ListWordsResponse.hpp"
#include "responses/LookupResponse.hpp"
#include "responses/ModifyResponse.hpp"
#include "responses/SearchDeleteResponse.hpp"
#include "responses/SearchResponse.hpp"
#include "responses/StatusResponse.hpp"

#endif //#ifndef CPS_API_HPP 
