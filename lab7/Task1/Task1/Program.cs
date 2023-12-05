using System.Text;
class Program
{
    static async Task Main()
    {
        Console.WriteLine("Enter a file path:");
        string filePath = Console.ReadLine();

        Console.WriteLine("Enter a characters to remove:");
        string charactersToRemove = Console.ReadLine();

        try
        {
            string text = await ReadTextFromFileAsync(filePath);
            string cleanedText = RemoveCharacters(text, charactersToRemove);

            await SaveTextToFileAsync(filePath, cleanedText);

            Console.WriteLine("Success");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"{ex.Message}");
        }
    }

    static async Task<string> ReadTextFromFileAsync(string filePath)
    {
        using (StreamReader reader = new StreamReader(filePath))
        {
            return await reader.ReadToEndAsync();
        }
    }

    static string RemoveCharacters(string text, string charactersToRemove)
    {
        StringBuilder builder = new StringBuilder(text);

        foreach (char character in charactersToRemove)
        {
            builder.Replace(character.ToString(), string.Empty);
        }

        return builder.ToString();
    }

    static async Task SaveTextToFileAsync(string filePath, string text)
    {
        using (StreamWriter writer = new StreamWriter(filePath))
        {
            await writer.WriteAsync(text);
        }
    }
}
