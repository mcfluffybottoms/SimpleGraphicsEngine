# SimpleEngine

### Системные требования

- Vulkan
- Qt6
- Библиотеки glm и tiny_obj_loader

Shared_ptr используются для мешей и текстур - вещей, для которых потенциально нужно использовать несколько указателей (несколько указателей "делят" между собой один объект). К примеру, когда несколько объектов используют один меш.

Unique_ptr в данном проекте используются для определяется менеджеров ресурса и менеджера мира. Лишь один указатель имеет владение над объектом. Так обеспечивается единоличное владение объектом.