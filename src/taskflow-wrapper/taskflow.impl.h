#pragma once
#include <memory>

namespace taskflow { namespace dotnet { namespace impl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_factory;
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	struct pimpl_wrapper {
		// virtual ~pimpl_wrapper();
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class pimpl_container {
	public:
		using wrapper_ptr = pimpl_wrapper*;
		using base = pimpl_container;

		pimpl_container(wrapper_ptr ptr);
		~pimpl_container();
	protected:

		wrapper_ptr _pimpl_ptr;
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class submitted_task: public pimpl_container
	{
	private:
		submitted_task(wrapper_ptr ptr);
	public:
		const submitted_task * precede(const submitted_task *);
		const submitted_task * succede(const submitted_task *);

		friend class task_factory;
		friend class task_flow;

		template<typename container_t, typename wrapped_t>
		friend container_t * get_result(wrapped_t* ptr);

	};
	struct task_action {
		virtual void run() = 0;
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_executor;
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_flow: public pimpl_container {
	private:
		task_flow(wrapper_ptr ptr);
	public:
		submitted_task * emplace(std::shared_ptr<task_action> );


		friend class task_factory;
		friend class task_executor;
		template<typename container_t, typename wrapped_t>
		friend container_t * get_result(wrapped_t* ptr);
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_status : public pimpl_container
	{
	private:
		task_status(wrapper_ptr ptr);
	public:
		enum class wait_result {
			ready,
			timeout,
			deferred
		};
		void wait();
		wait_result wait_until(std::size_t millisecs);

		friend class task_factory;
		friend class task_executor;

		template<typename container_t, typename wrapped_t>
		friend container_t * get_result(wrapped_t* ptr);
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_executor : public pimpl_container {
	private:
		task_executor(wrapper_ptr ptr);
	public:
		task_status * run(const task_flow &);


		friend class task_factory;
		template<typename container_t, typename wrapped_t>
		friend container_t * get_result(wrapped_t* ptr);
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++
	class task_factory {
	private:
		task_factory();
	public:
		static task_executor * new_executor();
		static task_flow * new_flow();
	};
	//+++++++++++++++++++++++++++++++++++++++++++++++++

} } } // namespace