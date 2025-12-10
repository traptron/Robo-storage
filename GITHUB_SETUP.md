# 🚀 Инструкция: Выгрузка проекта на GitHub

Проект **Robo-Storage** инициализирован и готов к загрузке на GitHub.

## ✅ Что уже сделано локально

- ✓ Инициализирован git репозиторий
- ✓ Добавлены все файлы (39 файлов, 3103 строки кода)
- ✓ Создан первый коммит `Initial commit`
- ✓ Ветка `main` активна

**Текущее состояние:**
```
commit 8f67ea6
Author: Robo-Storage Team <admin@robo-storage.local>
Date:   10 Dec 2025

    Initial commit: Robo-Storage project scaffold with ROS2 Jazzy, SLAM, Nav2, and ESP32 firmware
```

---

## 📝 Пошаговая инструкция для GitHub

### Шаг 1: Создать пустой репозиторий на GitHub

1. Зайти на [GitHub.com](https://github.com)
2. Нажать **"+"** → **"New repository"**
3. Назвать репозиторий: `Robo-storage` (или любое другое имя)
4. Описание: `Autonomous warehouse system with ROS2 Jazzy, SLAM, Nav2, and ESP32`
5. Выбрать **Public** (для open-source)
6. **НЕ инициализировать** с README, .gitignore, license (уже есть локально)
7. Нажать **"Create repository"**

### Шаг 2: Скопировать URL репозитория

После создания репозитория GitHub покажет команды. Скопировать ссылку формата:
```
https://github.com/YOUR-USERNAME/Robo-storage.git
```
или (если используется SSH):
```
git@github.com:YOUR-USERNAME/Robo-storage.git
```

### Шаг 3: Добавить удалённый репозиторий локально

**Вариант A: HTTPS (проще для начинающих)**

```bash
cd c:\Users\adm\Desktop\Project\Robo-storage

git remote add origin https://github.com/YOUR-USERNAME/Robo-storage.git
git branch -M main
git push -u origin main
```

**Вариант B: SSH (требует настройки ключей)**

```bash
cd c:\Users\adm\Desktop\Project\Robo-storage

git remote add origin git@github.com:YOUR-USERNAME/Robo-storage.git
git branch -M main
git push -u origin main
```

### Шаг 4: Аутентификация GitHub

#### Для HTTPS:
При первом push потребуется аутентификация:
- На GitHub перейти: Settings → Developer settings → Personal access tokens
- Нажать "Generate new token (classic)"
- Выбрать scopes: `repo`, `workflow`
- Скопировать token
- При запросе пароля в git вставить token вместо пароля

#### Для SSH:
[Инструкция по настройке SSH ключей](https://docs.github.com/en/authentication/connecting-to-github-with-ssh)

### Шаг 5: Выполнить push

```bash
# Проверить удалённый репозиторий
git remote -v

# Выполнить push
git push -u origin main
```

**Если используется PowerShell и возникают проблемы, попробовать:**
```powershell
git push -u origin main 2>&1
```

---

## ✨ После успешного push

### Проверить на GitHub:
1. Открыть репозиторий в браузере
2. Убедиться, что видны все файлы
3. История коммитов показана

### Создать дополнительные ветки (опционально):

```bash
# Создать ветку dev
git checkout -b dev
git push -u origin dev

# Создать ветку для Фазы 1
git checkout -b feature/phase1-mobility
git push -u origin feature/phase1-mobility

# Вернуться на main
git checkout main
```

### Настроить GitHub (опционально):

1. **Защита ветки main:**
   - Settings → Branches → Add rule
   - Branch name pattern: `main`
   - Require pull request reviews before merging: ✓

2. **Добавить описание проекта:**
   - Settings → Description: "Autonomous warehouse system with ROS2 Jazzy"
   - Topics: `ros2`, `slam`, `navigation`, `robotics`, `autonomous`
   - Website: (если есть)

3. **Добавить лицензию:**
   - Нажать `Add file` → `Create new file`
   - Назвать `LICENSE`
   - Выбрать MIT License template

4. **Настроить GitHub Pages (документация):**
   - Settings → Pages
   - Source: Deploy from a branch
   - Branch: main, folder: /docs (если создана)

---

## 📊 Проверка после push

```bash
# Проверить статус
git status
# Output: On branch main, nothing to commit

# Проверить логи
git log --oneline
# Output: 8f67ea6 Initial commit: Robo-Storage project scaffold...

# Проверить удалённый репозиторий
git remote -v
# Output:
# origin  https://github.com/YOUR-USERNAME/Robo-storage.git (fetch)
# origin  https://github.com/YOUR-USERNAME/Robo-storage.git (push)
```

---

## 🔄 Регулярные операции

### Добавить изменения:
```bash
git add .
git commit -m "Описание изменений"
git push
```

### Загрузить последние изменения с GitHub:
```bash
git pull origin main
```

### Переключиться на другую ветку:
```bash
git checkout dev
git pull origin dev
```

---

## ⚠️ Решение проблем

### "fatal: not a valid object name: 'main'"
Решение: ветка ещё не создана, используется `master`
```bash
git branch -M main
git push -u origin main
```

### "error: failed to push some refs to origin"
Решение: GitHub уже содержит коммиты
```bash
git pull origin main --rebase
git push -u origin main
```

### "Authentication failed"
Решение: проверить token или SSH ключ
- HTTPS: Settings → Developer settings → Personal access tokens
- SSH: Settings → SSH and GPG keys

### "Permission denied (publickey)" при SSH
Решение: добавить публичный ключ на GitHub
```bash
# Вывести публичный ключ
cat ~/.ssh/id_rsa.pub

# Скопировать в GitHub Settings → SSH and GPG keys → New SSH key
```

---

## 📋 Чеклист

- [ ] Создан пустой репозиторий на GitHub
- [ ] Скопирована ссылка репозитория (HTTPS или SSH)
- [ ] Выполнена команда `git remote add origin ...`
- [ ] Выполнена команда `git push -u origin main`
- [ ] Проверен репозиторий в браузере (все файлы видны)
- [ ] Создана лицензия (LICENSE файл)
- [ ] Настроена защита ветки main (опционально)
- [ ] Добавлены topics: ros2, slam, robotics (опционально)

---

## 💡 Советы

1. **Используйте .gitignore** — уже настроен в проекте
2. **Пишите понятные commit messages** — помогает отслеживать историю
3. **Делайте регулярные pushes** — не давайте большие накопления коммитов
4. **Используйте ветки** — main для стабильного кода, dev/feature для разработки
5. **Документируйте README** — помогает новым разработчикам понять проект

---

## 📚 Дополнительные ресурсы

- [GitHub Docs: Adding locally hosted code to GitHub](https://docs.github.com/en/migrations/importing-source-code/using-the-command-line-to-import-source-code/adding-locally-hosted-code-to-github)
- [Git Documentation](https://git-scm.com/doc)
- [GitHub Guides](https://guides.github.com/)

---

**После выполнения всех шагов проект будет доступен на GitHub! 🎉**
