document.addEventListener('DOMContentLoaded', async () => {
    // --- DOM Elements ---
    const dateTimeDisplay = document.getElementById('dateTimeDisplay');
    const timerDisplay = document.getElementById('timerDisplay');
    const pomodoroButton = document.getElementById('pomodoroButton');
    const breakButton = document.getElementById('breakButton');
    const settingsButton = document.getElementById('settingsButton');
    const closeSettingsButton = document.getElementById('closeSettingsButton');
    const settingsModal = document.getElementById('settingsModal');
    const modalOverlay = document.getElementById('modalOverlay');
    const pomoDurationInput = document.getElementById('pomoDuration');
    const breakDurationInput = document.getElementById('breakDuration');
    const taskDateDisplay = document.getElementById('taskDateDisplay');
    const taskListContainer = document.getElementById('taskListContainer'); // Container includes header/input
    const taskListElement = document.getElementById('taskList'); // The <ul> element
    const totalPomosTodayDisplay = document.getElementById('totalPomosToday');
    const startTimeInput = document.getElementById('startTime');
    const setStartTimeButton = document.getElementById('setStartTimeButton');
    const clearStartTimeButton = document.getElementById('clearStartTimeButton');
    const startTimeMessage = document.getElementById('startTimeMessage');
    const messageBox = document.getElementById('messageBox');
    const messageText = document.getElementById('messageText');
    const messageCloseButton = document.getElementById('messageCloseButton');
    const messageBoxButtons = document.getElementById('messageBoxButtons'); // Container for buttons
    const newTaskInput = document.getElementById('newTaskInput');
    const pomoCountInput = document.getElementById('pomoCountInput'); // New input for anticipated pomos
    const addTaskButton = document.getElementById('addTaskButton');
    const totalTaskTimeDisplay = document.getElementById('totalTaskTimeDisplay'); // New element for total time

    // --- Sound Effect ---
    const timerCompleteSound = new Audio('https://assets.mixkit.co/sfx/preview/mixkit-alert-quick-chime-766.mp3');

    // --- State ---
    let timerInterval = null;
    let timerSeconds = 0;
    let currentMode = 'Focus'; // 'Focus' or 'BREAK'
    let isRunning = false;
    let scheduledStartTime = localStorage.getItem('pomotree_scheduledStartTime') || null;
    let scheduledIntervalCheck = null;
    let totalPomosToday = parseInt(localStorage.getItem('pomotree_totalPomosToday') || '0');
    let tasks = JSON.parse(localStorage.getItem('pomotree_tasks') || '[]');
    let selectedTaskId = localStorage.getItem('pomotree_selectedTaskId') || null;
    let nextTimerCallback = null; // Callback to run after message box closes

    // --- Settings ---
    let pomoDuration = parseInt(localStorage.getItem('pomotree_pomoDuration') || '25');
    let breakDuration = parseInt(localStorage.getItem('pomotree_breakDuration') || '5');

    // --- Initialization ---
    async function initialize() {
        updateDateTime();
        setInterval(updateDateTime, 1000);

        pomoDurationInput.value = pomoDuration;
        breakDurationInput.value = breakDuration;
        totalPomosTodayDisplay.textContent = totalPomosToday;

        loadTasks(); // Load and render tasks
        resetTimer(); // Set initial timer display
        updateTotalTaskTime(); // Calculate and display total task time

        // Display and check scheduled start time
        if (scheduledStartTime) {
            const [hours, minutes] = scheduledStartTime.split(':');
            startTimeInput.value = scheduledStartTime;
            startTimeMessage.textContent = `Timer scheduled for ${formatTimeAMPM(hours, minutes)}.`;
            startScheduledCheck();
        }

        addEventListeners();

        // Setup exit warning
        window.addEventListener('beforeunload', (e) => {
            const message = 'You have unsaved changes. Are you sure you want to leave?';
            e.returnValue = message;
            return message;
        });

        // Setup mouse leave event
        document.body.addEventListener('mouseleave', () => {
            document.body.classList.add('text-hidden');
        });

        // Setup mouse enter event
        document.body.addEventListener('mouseenter', () => {
            document.body.classList.remove('text-hidden');
        });

        // await initTreeGenerator();
    }

    function addEventListeners() {
        pomodoroButton.addEventListener('click', () => startTimer('Focus'));
        breakButton.addEventListener('click', () => startTimer('Break'));
        settingsButton.addEventListener('click', openSettings);
        closeSettingsButton.addEventListener('click', closeSettings);
        modalOverlay.addEventListener('click', closeSettings);
        pomoDurationInput.addEventListener('change', updateDurations);
        breakDurationInput.addEventListener('change', updateDurations);
        setStartTimeButton.addEventListener('click', setScheduledStartTime);
        clearStartTimeButton.addEventListener('click', clearScheduledStartTime);
        messageCloseButton.addEventListener('click', closeMessageBox);
        addTaskButton.addEventListener('click', handleAddTask);
        newTaskInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                handleAddTask();
            }
        });

        // Add event listeners for date navigation buttons (should be disabled)
        const prevDateButton = taskDateDisplay.querySelector('.prev-date') || 
                              document.createElement('span');
        prevDateButton.classList.add('date-nav', 'prev-date', 'disabled');
        prevDateButton.textContent = '<';
        prevDateButton.title = 'Previous day (Premium feature)';
        
        const nextDateButton = taskDateDisplay.querySelector('.next-date') || 
                              document.createElement('span');
        nextDateButton.classList.add('date-nav', 'next-date', 'disabled');
        nextDateButton.textContent = '>';
        nextDateButton.title = 'Next day (Premium feature)';
        
        // If they don't exist yet, wrap the date display text and append buttons
        if (!taskDateDisplay.querySelector('.date-nav')) {
            const dateText = taskDateDisplay.textContent;
            taskDateDisplay.textContent = '';
            taskDateDisplay.appendChild(prevDateButton);
            
            const dateSpan = document.createElement('span');
            dateSpan.textContent = dateText.replace('<', '').replace('>', '');
            taskDateDisplay.appendChild(dateSpan);
            
            taskDateDisplay.appendChild(nextDateButton);
        }

        // Event delegation for task list items (select, delete, checkbox)
        taskListElement.addEventListener('click', (event) => {
            const taskItem = event.target.closest('.task-item');
            if (!taskItem) return; // Click wasn't on a task item or its children

            const taskId = taskItem.dataset.id;

            // Handle delete button click
            if (event.target.classList.contains('delete-button')) {
                deleteTask(taskId);
                return; // Stop further processing
            }

            // Handle checkbox click (specific target)
            if (event.target.type === 'checkbox') {
                handleTaskCheck(taskId, event.target.checked);
                // Don't select the task if only the checkbox was clicked
                return;
            }

            // Handle task selection (click anywhere else on the item)
            selectTask(taskId);
        });
    }


    // --- Clock ---
    function updateDateTime() {
        const now = new Date();
        const day = String(now.getDate()).padStart(2, '0');
        const month = String(now.getMonth() + 1).padStart(2, '0'); // Months are 0-based
        const year = now.getFullYear();
        const hours = String(now.getHours()).padStart(2, '0');
        const minutes = String(now.getMinutes()).padStart(2, '0');
        const seconds = String(now.getSeconds()).padStart(2, '0');

        dateTimeDisplay.textContent = `[ ${day}/${month}/${year} ${hours}:${minutes}:${seconds} ]`;
        // Update task date display only if it hasn't been updated today
        if (!taskDateDisplay.dataset.updatedToday || taskDateDisplay.dataset.updatedToday !== `${day}/${month}`) {
            const prevButton = taskDateDisplay.querySelector('.prev-date') || document.createElement('span');
            const nextButton = taskDateDisplay.querySelector('.next-date') || document.createElement('span');
            
            const dateSpan = taskDateDisplay.querySelector('span:not(.date-nav)') || document.createElement('span');
            dateSpan.textContent = `[ ${day}/${month} Tasks ]`;
            
            taskDateDisplay.dataset.updatedToday = `${day}/${month}`;
        }
    }

    // --- Timer ---
    function startTimer(mode) {
        if (isRunning && currentMode === mode) return;
    
        clearInterval(timerInterval);
        if (scheduledIntervalCheck) {
            clearInterval(scheduledIntervalCheck);
            scheduledIntervalCheck = null;
        }
    
        currentMode = mode;
        timerSeconds = (mode === 'Focus' ? pomoDuration : breakDuration) * 60;
        isRunning = true;
        nextTimerCallback = null;
    
        updateTimerDisplay();
        timerInterval = setInterval(tick, 1000);
    
        pomodoroButton.classList.toggle('active', mode === 'Focus');
        breakButton.classList.toggle('active', mode === 'Break');
        
        // // Generate a new tree when starting a Focus session
        // if (mode === 'Focus') {
        //     generateTree();
        // }
    }

    function tick() {
        if (timerSeconds <= 0) {
            timerComplete();
            return;
        }
        timerSeconds--;
        updateTimerDisplay();
    }

    function timerComplete() {
        clearInterval(timerInterval);
        isRunning = false;
        const completedMode = currentMode; // Store mode before resetting

        // Play sound
        timerCompleteSound.play();

        // Increment pomodoro count for selected task if Focus session completed
        if (completedMode === 'Focus' && selectedTaskId) {
            const taskIndex = tasks.findIndex(t => t.id == selectedTaskId); // Use == for type coercion if needed
            if (taskIndex > -1 && !tasks[taskIndex].completed) {
                tasks[taskIndex].pomos = (tasks[taskIndex].pomos || 0) + 1;
                updateTaskElement(tasks[taskIndex]); // Update DOM
                saveTasks(); // Save to localStorage
            }
             // Increment total pomos for the day
             totalPomosToday++;
             totalPomosTodayDisplay.textContent = totalPomosToday;
             localStorage.setItem('pomotree_totalPomosToday', totalPomosToday.toString());
             updateTotalTaskTime();
        }

        // Determine next mode
        const nextMode = completedMode === 'Focus' ? 'Break' : 'Focus';

        // Set up the callback to start the next timer after user clicks OK
        nextTimerCallback = () => startTimer(nextMode);

        // Show message box
        showMessageBox(`${completedMode} session complete! Time for a ${nextMode}?`);

        // Reset timer display visually, but don't start next timer yet
        resetTimerVisuals();
    }

    // Resets timer display and buttons without clearing intervals/state fully
    function resetTimerVisuals() {
         currentMode = 'Focus'; // Visually reset to default
         timerSeconds = pomoDuration * 60;
         updateTimerDisplay();
         pomodoroButton.classList.remove('active');
         breakButton.classList.remove('active');
         document.title = "Pomotree"; // Reset title
    }

    // Full reset (e.g., on init or manual stop)
    function resetTimer() {
         clearInterval(timerInterval);
         isRunning = false;
         nextTimerCallback = null;
         resetTimerVisuals(); // Reset display
          // Restart scheduled check if needed and not running
         if (scheduledStartTime && !scheduledIntervalCheck && !isRunning) {
             startScheduledCheck();
         }
    }


    function updateTimerDisplay() {
        const minutes = Math.floor(timerSeconds / 60);
        const seconds = timerSeconds % 60;
        const displayMode = isRunning ? currentMode : 'Focus'; // Show Focus if idle
        const displayMinutes = isRunning ? minutes : pomoDuration;
        const displaySeconds = isRunning ? seconds : 0;

        // Update the main timer display text
        timerDisplay.textContent = `[ ${displayMode} ${String(displayMinutes).padStart(2, '0')}:${String(displaySeconds).padStart(2, '0')} ]`;

        // Update the document title (browser tab)
        if (isRunning) {
             // Format: (MM:SS) MODE - Pomotree
            document.title = `(${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}) ${currentMode} - Pomotree`;
        } else {
            // Reset title when timer is not running
            document.title = "Pomotree";
        }
    }

    // --- Settings ---
    function openSettings() {
        settingsModal.classList.remove('hidden');
        modalOverlay.classList.remove('hidden');
    }

    function closeSettings() {
        settingsModal.classList.add('hidden');
        modalOverlay.classList.add('hidden');
        updateDurations(); // Save durations when closing
    }

    function updateDurations() {
        pomoDuration = parseInt(pomoDurationInput.value) || 25;
        breakDuration = parseInt(breakDurationInput.value) || 5;

        if (pomoDuration < 1) pomoDuration = 1;
        if (breakDuration < 1) breakDuration = 1;

        pomoDurationInput.value = pomoDuration;
        breakDurationInput.value = breakDuration;

        localStorage.setItem('pomotree_pomoDuration', pomoDuration.toString());
        localStorage.setItem('pomotree_breakDuration', breakDuration.toString());

        if (!isRunning) {
            resetTimer(); // Update display if timer is idle
        }
        
        updateTotalTaskTime(); // Recalculate total task time with new durations
    }

    // --- Total Task Time Calculation ---
    function updateTotalTaskTime() {
        // Count total anticipated pomos across all uncompleted tasks
        const totalAnticipatedPomos = tasks
            .filter(task => !task.completed)
            .reduce((sum, task) => sum + (task.anticipatedPomos || 0), 0);
        
        // Calculate total time in minutes
        const totalTimeMinutes = (totalAnticipatedPomos * pomoDuration) + 
                               ((totalAnticipatedPomos - 1) * breakDuration); // Breaks between pomos
        
        // Convert to hours and minutes
        const hours = Math.floor(totalTimeMinutes / 60);
        const minutes = totalTimeMinutes % 60;
        
        // Display the total
        if (totalTaskTimeDisplay) {
            totalTaskTimeDisplay.textContent = `Total estimated time: ${hours > 0 ? hours + 'h ' : ''}${minutes}m (${totalAnticipatedPomos} pomos)`;
        }
    }

    // --- Scheduled Start Time ---
    function setScheduledStartTime() {
        const timeValue = startTimeInput.value;
        if (timeValue) {
            scheduledStartTime = timeValue;
            localStorage.setItem('pomotree_scheduledStartTime', scheduledStartTime);
            const [hours, minutes] = scheduledStartTime.split(':');
            startTimeMessage.textContent = `Timer scheduled for ${formatTimeAMPM(hours, minutes)}.`;
            startScheduledCheck();
            showMessageBox(`Pomodoro timer scheduled for ${formatTimeAMPM(hours, minutes)}.`);
        } else {
            startTimeMessage.textContent = "Please select a valid time.";
        }
    }

    function clearScheduledStartTime() {
        scheduledStartTime = null;
        localStorage.removeItem('pomotree_scheduledStartTime');
        startTimeInput.value = '';
        startTimeMessage.textContent = '';
        if (scheduledIntervalCheck) {
            clearInterval(scheduledIntervalCheck);
            scheduledIntervalCheck = null;
        }
        showMessageBox("Scheduled start time cleared.");
    }

    function startScheduledCheck() {
        if (scheduledIntervalCheck) clearInterval(scheduledIntervalCheck);
        if (!scheduledStartTime) return;

        // Check immediately once
        checkSchedule();

        // Then check every minute
        scheduledIntervalCheck = setInterval(checkSchedule, 1000); // Check every second
    }

    function checkSchedule() {
        if (!scheduledStartTime) { // Stop if cleared or timer already running
            if (scheduledIntervalCheck) clearInterval(scheduledIntervalCheck);
            scheduledIntervalCheck = null;
            return;
        }
   
        const now = new Date();
        const currentHours = now.getHours();
        const currentMinutes = now.getMinutes();
        const [scheduledHours, scheduledMinutes] = scheduledStartTime.split(':').map(Number);
   
        // Check if the current time matches the scheduled time
        if (currentHours === scheduledHours && currentMinutes === scheduledMinutes) {
            // It's time!
            const scheduleTimeFormatted = formatTimeAMPM(scheduledHours, scheduledMinutes); // Store before clearing
   
            // Clear schedule state *before* showing message/starting timer
            if (scheduledIntervalCheck) clearInterval(scheduledIntervalCheck);
            scheduledIntervalCheck = null;
            scheduledStartTime = null;
            localStorage.removeItem('pomotree_scheduledStartTime');
            startTimeInput.value = '';
            startTimeMessage.textContent = '';
   
            // Always show message box with start button
            showMessageBox(`It's time for your scheduled Focus session (${scheduleTimeFormatted})!`, true);
        }
   }


    // --- Task List ---
    function saveTasks() {
        localStorage.setItem('pomotree_tasks', JSON.stringify(tasks));
        updateTotalTaskTime(); // Update the total time when tasks change
    }

    function loadTasks() {
        taskListElement.innerHTML = ''; // Clear existing list
        tasks.forEach(task => {
            const taskElement = createTaskElement(task);
            taskListElement.appendChild(taskElement);
        });
        // Re-apply selection style if a task was selected
        if (selectedTaskId) {
            const selectedElement = taskListElement.querySelector(`.task-item[data-id="${selectedTaskId}"]`);
            if (selectedElement) {
                selectedElement.classList.add('selected-task');
            } else {
                // Selected task no longer exists, clear selection
                selectedTaskId = null;
                localStorage.removeItem('pomotree_selectedTaskId');
            }
        }
    }

    function createTaskElement(task) {
        const li = document.createElement('li');
        li.className = `task-item ${task.completed ? 'completed' : ''}`;
        li.dataset.id = task.id;

        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = task.completed;
        checkbox.id = `task-${task.id}`; // Unique ID for label association (optional)

        const span = document.createElement('span');
        span.className = 'task-name';
        span.textContent = task.name;

        const pomosSpan = document.createElement('span');
        pomosSpan.className = 'pomos-count';
        pomosSpan.textContent = `Pomos: ${task.pomos || 0}/${task.anticipatedPomos || '?'}`;

        const deleteButton = document.createElement('button');
        deleteButton.className = 'delete-button';
        deleteButton.innerHTML = '&times;'; // 'x'
        deleteButton.title = "Delete task";


        li.appendChild(checkbox);
        li.appendChild(span);
        li.appendChild(pomosSpan);
        li.appendChild(deleteButton);

        // Add selected class if this is the selected task
        if (task.id == selectedTaskId) { // Use == for potential type difference
             li.classList.add('selected-task');
        }


        return li;
    }

     // Updates the display of an existing task element in the DOM
    function updateTaskElement(task) {
        const taskElement = taskListElement.querySelector(`.task-item[data-id="${task.id}"]`);
        if (taskElement) {
            taskElement.classList.toggle('completed', task.completed);
            const pomosSpan = taskElement.querySelector('.pomos-count');
            if (pomosSpan) {
                pomosSpan.textContent = `Pomos: ${task.pomos || 0}/${task.anticipatedPomos || '?'}`;
            }
             const checkbox = taskElement.querySelector('input[type="checkbox"]');
            if (checkbox) {
                checkbox.checked = task.completed;
            }
        }
    }


    function handleAddTask() {
        const taskName = newTaskInput.value.trim();
        if (taskName) {
            // Get the anticipated pomos value with fallback
            const anticipatedPomos = parseInt(pomoCountInput?.value) || 1;
            
            const newTask = {
                id: Date.now(), // Simple unique ID
                name: taskName,
                completed: false,
                pomos: 0,
                anticipatedPomos: anticipatedPomos
            };
            tasks.push(newTask);
            const taskElement = createTaskElement(newTask);
            taskListElement.appendChild(taskElement);
            saveTasks();
            newTaskInput.value = ''; // Clear input
            if (pomoCountInput) pomoCountInput.value = '1'; // Reset to default
        }
    }

    function deleteTask(taskId) {
        tasks = tasks.filter(task => task.id != taskId); // Use != for type coercion just in case
        const taskElement = taskListElement.querySelector(`.task-item[data-id="${taskId}"]`);
        if (taskElement) {
            taskElement.remove();
        }
        // If the deleted task was selected, clear selection
        if (selectedTaskId == taskId) {
            selectedTaskId = null;
            localStorage.removeItem('pomotree_selectedTaskId');
        }
        saveTasks();
    }

    function selectTask(taskId) {
        // Deselect previous task
        if (selectedTaskId) {
            const previousSelected = taskListElement.querySelector(`.task-item[data-id="${selectedTaskId}"]`);
            if (previousSelected) {
                previousSelected.classList.remove('selected-task');
            }
        }

        // Select new task
        const newSelected = taskListElement.querySelector(`.task-item[data-id="${taskId}"]`);
        if (newSelected) {
             // Don't select if it's already completed
             const task = tasks.find(t => t.id == taskId);
             if (task && !task.completed) {
                 newSelected.classList.add('selected-task');
                 selectedTaskId = taskId;
                 localStorage.setItem('pomotree_selectedTaskId', selectedTaskId);
             } else {
                 // Task is completed or not found, clear selection
                 selectedTaskId = null;
                 localStorage.removeItem('pomotree_selectedTaskId');
             }
        } else {
            // Task not found (shouldn't happen normally)
             selectedTaskId = null;
             localStorage.removeItem('pomotree_selectedTaskId');
        }
    }


    function handleTaskCheck(taskId, isCompleted) {
        const taskIndex = tasks.findIndex(t => t.id == taskId);
        if (taskIndex > -1) {
            tasks[taskIndex].completed = isCompleted;
            updateTaskElement(tasks[taskIndex]); // Update DOM visual state

             // If completing the currently selected task, deselect it
             if (isCompleted && selectedTaskId == taskId) {
                 const taskElement = taskListElement.querySelector(`.task-item[data-id="${taskId}"]`);
                 if (taskElement) {
                     taskElement.classList.remove('selected-task');
                 }
                 selectedTaskId = null;
                 localStorage.removeItem('pomotree_selectedTaskId');
             }

            saveTasks();
        }
    }

    // --- Message Box ---
    function showMessageBox(message, showStartButton = false) {
        messageText.textContent = message;

        // Clear previous dynamic buttons
        const existingStartBtn = document.getElementById('messageStartButton');
        if (existingStartBtn) {
            existingStartBtn.remove();
        }

        // Add start button conditionally
        if (showStartButton) {
            const startButton = document.createElement('button');
            startButton.id = 'messageStartButton';
            startButton.textContent = 'Start Pomodoro';
            startButton.onclick = () => {
                startTimer('Focus');
                closeMessageBox(); // Close after starting
            };
            // Insert before the OK button
            messageBoxButtons.insertBefore(startButton, messageCloseButton);
        }

        messageBox.classList.remove('hidden');
    }

    function closeMessageBox() {
        messageBox.classList.add('hidden');
        // Execute and clear the callback if it exists
        if (nextTimerCallback) {
            nextTimerCallback();
            nextTimerCallback = null;
        }
    }

    // --- Utility Functions ---
    function formatTimeAMPM(hours, minutes) {
        const h = parseInt(hours);
        const m = String(minutes).padStart(2, '0');
        const ampm = h >= 12 ? 'PM' : 'AM';
        const formattedHour = h % 12 === 0 ? 12 : h % 12;
        return `${formattedHour}:${m} ${ampm}`;
    }

    // --- Start the application ---
    await initialize();
});