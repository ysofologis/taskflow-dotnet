#include "pch.h"
#include "taskflow.managed.h"
#include "taskflow.impl.h"
#include <functional>
#include <vcclr.h>

namespace taskflow { namespace dotnet { namespace managed {
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	template <typename PointerT>
	private ref class PtrContainer {
	public:
		typedef PointerT					PtrType;
		typedef PtrContainer<PointerT>		Base;
		typedef std::shared_ptr<PtrType>	SharedPtr;
	protected:
		SharedPtr* _ptr;
	public:

		PtrContainer(SharedPtr ptr) {
			_ptr = new SharedPtr();
			*_ptr = std::move(ptr);
		}
		~PtrContainer() {
			if (_ptr) {
				delete _ptr;
                _ptr = nullptr;
			}
		}
		!PtrContainer() {
			if (_ptr) {
				delete _ptr;
                _ptr = nullptr;
            }
		}
		PointerT * get() { return _ptr->get(); }
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	private ref class SubmittedTask : public ISubmittedTask, public PtrContainer<impl::submitted_task> {
	public:
		SubmittedTask(SharedPtr newPtr) : Base(newPtr) { }

		virtual ISubmittedTask^ precede(ISubmittedTask^ existingTask) {
			auto exitstingT = (SubmittedTask^)existingTask;
			auto this_task = get()->precede(exitstingT->get());
			return this;
		}
		virtual ISubmittedTask^ succeed(ISubmittedTask^ existingTask) {
			auto exitstingT = (SubmittedTask^)existingTask;
			auto this_task = get()->succede(exitstingT->get());
			return this;
		}
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	struct task_wrapper : public impl::task_action {
		gcroot<Action^> act;
		virtual void run() {
			act->Invoke();
		}
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	private ref class TaskFlow : public ITaskFlow, public PtrContainer<impl::task_flow> {
	public:
		TaskFlow(SharedPtr newPtr) : Base(newPtr) { }
		virtual ISubmittedTask^ emplace(Action^ act) {
			auto task_action = std::make_shared<task_wrapper>();
			task_action->act = act;
			auto api_ptr = get()->emplace(task_action);
			std::shared_ptr<impl::submitted_task> ptr{ api_ptr };
			auto ret = gcnew SubmittedTask(ptr);
			return ret;
		}
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	private ref class ExecutionStatus : public IExecutionStatus, public PtrContainer<impl::task_status> {
	public:
		ExecutionStatus(SharedPtr newPtr) : Base(newPtr) { }
		virtual void wait() {
			get()->wait();
		}
		virtual WaitResult waitUntil(UInt32 timeout) {
			auto ret = get()->wait_until(timeout);
			return (WaitResult)ret;
		}
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	private ref class TaskExecutor : public ITaskExecutor, public PtrContainer<impl::task_executor> {
	public:
		TaskExecutor(SharedPtr newPtr) : Base(newPtr) { }
		virtual IExecutionStatus^ run(ITaskFlow^ taskFlow) {
			TaskFlow^ flow = (TaskFlow^)taskFlow;

			auto exec_status = get()->run(*flow->get());
			std::shared_ptr<impl::task_status> ptr{ exec_status };
			return gcnew ExecutionStatus(ptr);
		}
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ITaskFlow^ TaskFlowDotnet::newTaskFlow() {
		std::shared_ptr<impl::task_flow> ptr{ impl::task_factory::new_flow() };
		return gcnew TaskFlow(ptr);
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ITaskExecutor^ TaskFlowDotnet::newExecutor() {
		std::shared_ptr<impl::task_executor> ptr{ impl::task_factory::new_executor() };
		return gcnew TaskExecutor(ptr);
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

} } } // namespace