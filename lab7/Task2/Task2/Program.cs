using System.Diagnostics;

class Program
{
    static async Task Main()
    {
        const string apiUrl = "https://dog.ceo/api/breeds/image/random";
        const int numberOfRequests = 10;
        const string saveDirectory = "D:\\study\\parallel_programming\\ParallelProgramming\\lab7\\Task2\\Task2";

        Console.WriteLine("Асинхронное выполнение");
        await RunAsync(apiUrl, numberOfRequests, saveDirectory);

        Console.WriteLine("\nСинхронное выполнение");
        RunSync(apiUrl, numberOfRequests, saveDirectory);

        Console.ReadLine();
    }

    static async Task RunAsync(string apiUrl, int numberOfRequests, string saveDir)
    {
        Stopwatch asyncStopwatch = Stopwatch.StartNew();

        // Создаем список задач (Task) для асинхронных запросов
        List<Task> asyncTasks = new List<Task>();

        for (int i = 0; i < numberOfRequests; i++)
        {
            asyncTasks.Add(MakeRequestAsync(apiUrl, i + 1, "Asynchronous", saveDir));
        }

        // Дожидаемся выполнения всех асинхронных задач
        await Task.WhenAll(asyncTasks);

        asyncStopwatch.Stop();
        Console.WriteLine($"Асинхронное выполнение завершено за {asyncStopwatch.ElapsedMilliseconds} мс");
    }

    static async void RunSync(string apiUrl, int numberOfRequests, string saveDir)
    {
        Stopwatch syncStopwatch = Stopwatch.StartNew();

        for (int i = 0; i < numberOfRequests; i++)
        {
            // Выполняем синхронный запрос
            await MakeRequestAsync(apiUrl, i + 1, "Suncronium", saveDir);
        }

        syncStopwatch.Stop();
        Console.WriteLine($"Синхронное выполнение завершено за {syncStopwatch.ElapsedMilliseconds} мс");
    }

    static async Task MakeRequestAsync(string apiUrl, int requestNumber, string requestType, string saveDir)
    {
        using (HttpClient client = new HttpClient())
        {
            try
            {
                // Отправляем асинхронный запрос и получаем JSON-ответ
                string jsonResponse = await client.GetStringAsync(apiUrl);
                var responseObject = Newtonsoft.Json.JsonConvert.DeserializeObject<Response>(jsonResponse);

                if (responseObject != null && responseObject.Status == "success")
                {
                    string imageUrl = responseObject.Message.Replace("\\/", "/");
                    string fileName = $"{requestType}_dog_{requestNumber}.jpg";
                    string filePath = Path.Combine(saveDir, fileName);

                    using (var responseStream = await client.GetStreamAsync(imageUrl))
                    using (var fileStream = File.Create(filePath))
                    {
                        await responseStream.CopyToAsync(fileStream);
                    }

                    Console.WriteLine($"{requestType} request {requestNumber}: Image saved to {filePath}");
                }
                else
                {
                    Console.WriteLine($"{requestType} request {requestNumber}: Invalid response format");
                }
            }
            catch (HttpRequestException ex)
            {
                Console.WriteLine($"{requestType} запрос {requestNumber}: Ошибка - {ex.Message}");
            }
        }
    }



    public class Response
    {
        public string Message { get; set; }
        public string Status { get; set; }
    }
}
