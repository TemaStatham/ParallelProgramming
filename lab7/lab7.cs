using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Введите путь к текстовому файлу:");
        string filePath = Console.ReadLine();

        if (File.Exists(filePath))
        {
            Console.WriteLine("Введите список символов для удаления (без пробелов):");
            string charactersToRemove = Console.ReadLine();

            await ProcessFileAsync(filePath, charactersToRemove);

            Console.WriteLine("Операция завершена. Текст успешно обработан и сохранен в файле.");
        }
        else
        {
            Console.WriteLine("Файл не найден.");
        }
    }

    static async Task ProcessFileAsync(string filePath, string charactersToRemove)
    {
        try
        {
            string content = await ReadFileAsync(filePath);

            foreach (char character in charactersToRemove)
            {
                content = content.Replace(character.ToString(), string.Empty);
            }

            await SaveFileAsync(filePath, content);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка: {ex.Message}");
        }
    }

    static async Task<string> ReadFileAsync(string filePath)
    {
        using (StreamReader reader = new StreamReader(filePath))
        {
            return await reader.ReadToEndAsync();
        }
    }

    static async Task SaveFileAsync(string filePath, string content)
    {
        using (StreamWriter writer = new StreamWriter(filePath, false, Encoding.UTF8))
        {
            await writer.WriteAsync(content);
        }
    }
}