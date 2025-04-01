#include "pch.h"
#include "CppUnitTest.h"

#include "../Client/Commands.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(Client)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Assert::AreEqual(1, 1);
		}
	};

	TEST_CLASS(Server)
	{
	public:

		TEST_METHOD(UPLOAD_001)
		{
			UI::cmdHelp();
			Assert::AreEqual(1, 1);
		}

		TEST_METHOD(UPLOAD_002)
		{
			Assert::AreEqual(1, 1);
		}

		TEST_METHOD(UPLOAD_003)
		{
			Assert::AreEqual(1, 1);
		}
	};
}
