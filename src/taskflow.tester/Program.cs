// See https://aka.ms/new-console-template for more information
using taskflow.dotnet.managed;

void scenario_1() {
    var taskFlow = TaskFlowDotnet.newTaskFlow();
    var taskExecutor = TaskFlowDotnet.newExecutor();

    var A = taskFlow.emplace(() => Console.WriteLine("Task A"));
    var B = taskFlow.emplace(() => Console.WriteLine("Task B"));
    var C = taskFlow.emplace(() => Console.WriteLine("Task C"));
    var D = taskFlow.emplace(() => Console.WriteLine("Task D"));
    var _begin = taskFlow.emplace(() => Console.WriteLine("---Tasks Begin---"));
    var _end = taskFlow.emplace(() => Console.WriteLine("---Tasks End---"));


    A.precede(B).precede(C);  // A runs before B and C
    D.succeed(B).succeed(C);  // D runs after  B and C
    _begin.precede(A);
    _end.succeed(D);

    taskExecutor.run(taskFlow).wait();
    Console.WriteLine($"****** {nameof(scenario_1)} finished");
}
int MAX_TASKS = 10000;

void scenario_use_dotnet_tasks() {
    var taskList = new List<Task>();
    for (int i = 0; i < MAX_TASKS; i++)
    {
        taskList.Add(Task.Factory.StartNew((ix) => {
            Console.WriteLine($"Executed .net task {ix}");
            Thread.Sleep(0);
        }, i));
    }

    Task.WaitAll(taskList.ToArray());
    Console.WriteLine($"****** {nameof(scenario_use_dotnet_tasks)} finished");
}
void scenario_use_dotnet_threads()
{
    var taskList = new List<Thread>();
    for (int i = 0; i < MAX_TASKS; i++)
    {
        var th = new Thread((ix) => {
            Console.WriteLine($"Executed .net thread {ix}");
            Thread.Sleep(0);
        });
        taskList.Add(th);
        th.Start(i);
    }

    taskList.ForEach(t => t.Join());
    Console.WriteLine($"****** {nameof(scenario_use_dotnet_tasks)} finished");
}

void scenario_use_taskflow_tasks() {
    var taskFlow = TaskFlowDotnet.newTaskFlow();
    var taskExecutor = TaskFlowDotnet.newExecutor();

    Action<int> createTask = (ix) => {
        taskFlow.emplace(() => {
            Console.WriteLine($"Executed flow-task {ix}");
            Thread.Sleep(0);
        });
    };

    for(var i = 0; i < MAX_TASKS; i++)
    {
        createTask(i);
    }
    taskExecutor.run(taskFlow).wait();
    Console.WriteLine($"****** {nameof(scenario_use_taskflow_tasks)} finished");
}


void run_scenario(string scenario, Action action) {
    var watch = new System.Diagnostics.Stopwatch();
    watch.Start();
    action();
    watch.Stop();
    Console.WriteLine($"========= Execution time {watch.ElapsedMilliseconds}");
    Console.ReadKey();
}

void main(string [] args) {
    if (args.Length >= 1 )
    {
        var scenarios = args[0].Split(",").Select(x => int.Parse(x));

        if (args.Length >= 2)
        {
            MAX_TASKS = int.Parse(args[1]);
        }

        if (scenarios.Contains(1))
        {
            run_scenario($"running {MAX_TASKS} with .net tasks", () =>
            {
                scenario_use_dotnet_tasks();
            });
        }
        if (scenarios.Contains(2))
        {
            run_scenario($"running {MAX_TASKS} with .net threads", () =>
            {
                scenario_use_dotnet_threads();
            });
        }
        if (scenarios.Contains(3))
        {
            run_scenario($"running {MAX_TASKS} with task_flow", () =>
            {
                scenario_use_taskflow_tasks();
            });
        }

        Console.ReadKey();
    } else
    {
        Console.WriteLine("Please pass arguments delimeted by comma");
    }
    
}


main(args.Length < 1 ? new string[] { "3" } : args);
