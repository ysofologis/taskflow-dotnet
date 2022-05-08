// taskflow.impl.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <taskflow/taskflow.hpp>
#include "taskflow.impl.h"

namespace taskflow { namespace dotnet { namespace impl {

	struct underlying {
		using Future = tf::Future<void>;
		using Task = tf::Task;
		using Executor = tf::Executor;
		using Taskflow = tf::Taskflow;
	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	template<typename wrapped_t>
	class pimpl_wrapper_t: public pimpl_wrapper {
	public:
		pimpl_wrapper_t(): _ptr(nullptr) {}
		pimpl_wrapper_t(wrapped_t* ptr): _ptr(ptr) {}
		~pimpl_wrapper_t() {
			if (_ptr) {
				delete _ptr;
			}
		}
		wrapped_t * get_underlying() const {
			return _ptr;
		}
	private:
		wrapped_t * _ptr;
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	template<typename wrapped_t>
	wrapped_t * get_underlying(const pimpl_wrapper * wrapper) {
		return static_cast<const pimpl_wrapper_t<wrapped_t> *>(wrapper)->get_underlying();
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	template<typename container_t, typename wrapped_t>
	container_t * get_result(wrapped_t * ptr) {
		auto ret = new container_t(new pimpl_wrapper_t<wrapped_t>(ptr));
		return ret;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	pimpl_container::pimpl_container(wrapper_ptr ptr) {
		_pimpl_ptr = ptr;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	pimpl_container::~pimpl_container() {
		delete _pimpl_ptr;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	submitted_task::submitted_task(wrapper_ptr ptr): base(ptr) {
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	const submitted_task * submitted_task::precede(const submitted_task* lhs) {

		auto this_task = get_underlying<underlying::Task>(_pimpl_ptr);
		auto lhs_task = get_underlying<underlying::Task>(lhs->_pimpl_ptr);

		this_task->precede(*lhs_task);

		return this;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	const submitted_task * submitted_task::succede(const submitted_task* lhs) {

		auto this_task = get_underlying<underlying::Task>(_pimpl_ptr);
		auto lhs_task = get_underlying<underlying::Task>(lhs->_pimpl_ptr);

		this_task->succeed(*lhs_task);

		return this;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_status::task_status(wrapper_ptr ptr) : base(ptr) {}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void task_status::wait() {
		auto this_future = get_underlying<underlying::Future>(_pimpl_ptr);
		this_future->wait();
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_status::wait_result task_status::wait_until(std::size_t millisecs) {
		auto this_future = get_underlying<underlying::Future>(_pimpl_ptr);
		auto ret = this_future->wait_for(std::chrono::milliseconds(millisecs));
		return (task_status::wait_result)ret;
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		task_executor::task_executor(wrapper_ptr ptr): base(ptr) {}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_status * task_executor::run(const task_flow& lhs) {

		auto this_executor = get_underlying<underlying::Executor>(_pimpl_ptr);
		auto lhs_flow = get_underlying<underlying::Taskflow>(lhs._pimpl_ptr);

		// force move constructor
		auto future_ptr = new underlying::Future(this_executor->run(*lhs_flow));

		return get_result<task_status, underlying::Future>(future_ptr);
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_flow::task_flow(wrapper_ptr ptr) : base(ptr) {}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	submitted_task * task_flow::emplace(std::shared_ptr<task_action> action) {
		auto this_flow = get_underlying<underlying::Taskflow>(_pimpl_ptr);

		// force move constructor
		auto task_ptr = new underlying::Task( this_flow->emplace([action = action]() {
			action->run();
		}));

		return get_result<submitted_task, underlying::Task>(task_ptr);
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_factory::task_factory() {}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_executor * task_factory::new_executor() {
		return get_result<task_executor, underlying::Executor>(new underlying::Executor());
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	task_flow * task_factory::new_flow() {
		return get_result<task_flow, underlying::Taskflow>(new underlying::Taskflow());
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

}}} // namespace