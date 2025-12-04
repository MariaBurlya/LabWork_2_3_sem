import os
import random


def create_english_words_file(filename, target_size_mb):

    # Список английских слов
    english_words = [
        "the", "be", "to", "of", "and", "in", "that", "have", "it",
        "for", "not", "on", "with", "he", "as", "you", "do", "at", "this",
        "but", "his", "by", "from", "they", "we", "say", "her", "she", "or",
        "an", "will", "my", "one", "all", "would", "there", "their", "what",
        "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
        "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
        "people", "into", "year", "your", "good", "some", "could", "them", "see",
        "other", "than", "then", "now", "look", "only", "come", "its", "over",
        "think", "also", "back", "after", "use", "two", "how", "our", "work",
        "first", "well", "way", "even", "new", "want", "because", "any", "these",
        "give", "day", "most", "us", "data", "file", "system", "program", "code",
        "test", "computer", "software", "network", "application", "development"
    ]

    # Показываем путь к файлу
    full_path = os.path.abspath(filename)
    print(f"Создаю файл: {full_path}")
    print(f"Целевой размер: {target_size_mb} MB")

    target_size_bytes = target_size_mb * 1024 * 1024
    current_size = 0
    line_count = 0

    with open(filename, 'w', encoding='utf-8') as file:
        while current_size < target_size_bytes:
            # Случайное количество слов в строке (от 1 до 20)
            word_count = random.randint(1, 20)

            # Собираем строку из случайных слов
            line_words = [random.choice(english_words) for _ in range(word_count)]
            line_content = ' '.join(line_words)

            # Добавляем символ новой строки
            full_line = line_content + '\n'
            line_size = len(full_line.encode('utf-8'))

            # Записываем в файл
            file.write(full_line)
            current_size += line_size
            line_count += 1

            # Показываем прогресс каждые 10000 строк
            if line_count % 10000 == 0:
                progress = (current_size / target_size_bytes) * 100
                print(f"Прогресс: {progress:.1f}%")

    print(f"Готово!")
    print(f"Файл: {filename}")
    print(f"Размер: {current_size / (1024 * 1024):.2f} MB")
    print(f"Строк: {line_count}")
    print(f"Путь: {os.path.dirname(full_path)}")


# Создаем один файл размером 5 MB
create_english_words_file("list_of_words.txt", 5)
