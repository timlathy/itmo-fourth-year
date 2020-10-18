package itmo.se.lab3

import itmo.se.lab3.helpers.*
import itmo.se.lab3.pages.HomePage
import itmo.se.lab3.pages.JobSearchPage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class JobSearchTest {
    @TestTemplate
    @RunWithChrome
    fun `it allows the user to sort jobs by price`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val jobPage = JobSearchPage(driver)
        val jobs = jobPage.listItems()

        jobPage.sortSelector.selectByVisibleText("по цене")
        jobPage.waitForItems()
        val jobsByPrice = jobPage.listItems()

        assertTrue(jobsByPrice[0].price > jobs[0].price)
    }

    @TestTemplate
    @RunWithChrome
    fun `it allows the user to filter jobs by category`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val jobPage = JobSearchPage(driver)
        val jobs = jobPage.listItems()
        assertNotEquals("TEXT / Новая тема в форуме", jobs[0].getCategory())

        var categoryCheckbox = jobPage.getCategories().find { it.text == "TEXT / Новая тема в форуме" }!!
        assertFalse(categoryCheckbox.checked)
        categoryCheckbox.toggle()
        jobPage.searchButton.click()
        jobPage.waitForItems()

        categoryCheckbox = jobPage.getCategories().find { it.text == "TEXT / Новая тема в форуме" }!!
        assertTrue(categoryCheckbox.checked)

        val jobsByCategory = jobPage.listItems()
        for (job in jobsByCategory)
            assertEquals("TEXT / Новая тема в форуме", job.getCategory())
    }
}