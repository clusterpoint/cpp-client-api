#pragma once

#ifndef BASICIOTEST_HPP_
#define BASICIOTEST_HPP_

#include "TestCase.hpp"

class BasicIOTest : public TestCase
{
public:
	BasicIOTest(CPS::Connection& connection);

protected:
	virtual void run_tests();

private:
	void test_insert_one_document_and_delete_it();
	void test_insert_many_documents_and_delete_them();
	void test_insert_one_document_update_it_and_then_delete_it();
	void test_insert_many_documents_update_them_and_then_delete_them();
	void test_insert_one_document_retrieve_it_and_then_delete_it();
	void test_insert_many_documents_retrieve_them_and_then_delete_them();
	void test_insert_one_document_partially_replace_it_retrieve_it_and_then_delete_it();
	void test_insert_many_documents_partially_replace_them_retrieve_them_and_then_delete_them();
	void test_insert_many_documents_list_paths_and_delete_them();
	void test_insert_two_documents_list_last_and_delete_them();
	void test_insert_two_documents_lookup_last_and_delete_them();
	void test_insert_many_documents_search_and_delete_them();
};

#endif /* BASICIOTEST_HPP_ */
