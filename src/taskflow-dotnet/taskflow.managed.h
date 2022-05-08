#pragma once

using namespace System;
using namespace System::Threading::Tasks;

namespace taskflow { namespace dotnet { namespace managed {

			//+++++++++++++++++++++++++++++++++++++++++++++++++
			public interface class ISubmittedTask
			{
			public:
				virtual ISubmittedTask^ precede(ISubmittedTask^ existingTask);
				virtual ISubmittedTask^ succeed(ISubmittedTask^ existingTask);
			};
			//+++++++++++++++++++++++++++++++++++++++++++++++++
			public interface class ITaskFlow
			{
			public:
				virtual ISubmittedTask^ emplace(Action^ act);
			};
			//+++++++++++++++++++++++++++++++++++++++++++++++++
			public enum class WaitResult {
				ready,
				timeout,
				deferred
			};

			public interface class IExecutionStatus
			{
			public:
				virtual void wait();
				virtual WaitResult waitUntil(UInt32 timeout);
			};
			//+++++++++++++++++++++++++++++++++++++++++++++++++
			public interface class ITaskExecutor
			{
			public:
				virtual IExecutionStatus^ run(ITaskFlow^ taskFlow);
			};
			//+++++++++++++++++++++++++++++++++++++++++++++++++
			public ref class TaskFlowDotnet abstract sealed
			{
			public:
				static ITaskExecutor^ newExecutor();
				static ITaskFlow^ newTaskFlow();
			};
			//+++++++++++++++++++++++++++++++++++++++++++++++++

} } } // namespace
